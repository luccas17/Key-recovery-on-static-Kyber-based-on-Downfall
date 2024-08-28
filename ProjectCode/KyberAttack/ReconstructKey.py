import itertools

intersectionf = open("ProjectCode/KyberAttack/backgroundNoise.txt")
intersection = intersectionf.read().splitlines() 


def getMapsFromFile(f): 
  lines = f.readlines()

  maps = [list(group) for k, 
            group in
            itertools.groupby(lines, lambda x: x.startswith("Map")) if not k]
  for m in range(len(maps)):
    for i in range(len(maps[m])):
      line = maps[m][i]
      splits = line.split(" ")
      if(len(splits) > 2):
        maps[m][i] = [splits[1], splits[2]]
  return maps

# Used to find qword chains
def findNextQLink(index, prefix, maps, counter):
  acc = []
  for e in maps[index % 4]:
    word = e[0]
    if(not (len(word) == 24 and (prefix is None or prefix == word[:8]))):
      continue
    newPrefix = word[-8:]
    if(counter > 0):
      suffixes = findNextQLink(index+1, newPrefix, maps, counter-1)
      for suffix in suffixes:
        acc.append([word[:16]] + suffix)
    else:
      acc.append([word[:16]])
  return acc

def findChainsQ(maps):
  chains = []
  for i in range(len(maps)):
    acc = findNextQLink(i, None, maps, 3)
    for chainQ in acc:
      if (len(chainQ) == 4):
          chainQ = chainQ[-i:] + chainQ[:-i]
          avoidDuplicatesAppend(chainQ, chains)
  return chains

# Finds all length 24 hits in map that starts with prefix. Used to find dword chains
def findAllMatching(prefix, map):
  acc = []
  for line in map:
    e = line[0]
    if(e[:len(prefix)] == prefix and len(e) == 24):
      acc.append(e)
  return acc

def findNextDLink(index, prefix, maps, counter):
  # Prefix is 2-dwords long
  acc = []
  candidates = findAllMatching(prefix, maps[index % 8])
  # Assume that a lot of candidates means noise, not correct value
  if(len(candidates) > 10):
    return acc
  for candidate in candidates:
    if(candidate[-2:] == "00"):
      skip = False
      for c in candidates:
        if(c[:-2] == candidate[:-2] and c != candidate):
          skip = True
      if skip: continue
    if(counter > 0):
      suffixes = findNextDLink(index+1, candidate[8:], maps, counter-1)
      for suffix in suffixes:
        acc.append([prefix[:8]] + suffix)
    else:
      acc.append([candidate[:8], candidate[8:16], candidate[16:]])

  # Skip a segment of 3 dwords 
  gapCandidates = findAllMatching(prefix[8:], maps[(index+1) % 8])
  # Assume that a lot of candidates means noise, not correct value
  if(len(gapCandidates) > 10):
    return acc
  for gapCandidate in gapCandidates:
    if(counter > 0):
      suffixes = findNextDLink(index+2, gapCandidate[8:], maps, counter-2)
      for suffix in suffixes:
        acc.append([prefix[:8], prefix[8:16]] + suffix)
    else:
      acc.append([prefix[:8], gapCandidate[:8], gapCandidate[8:16]])
  return acc

def findChainsD(maps):
  chains = []
  for i in range(len(maps)):
    for line in maps[i]:
      word = line[0]
      if(len(word) != 24):
        continue
      candidates = findNextDLink(i+1, word[8:], maps, 4)
      for candidate in candidates:
        chain = [word[:8]] + candidate
        chain = list(dict.fromkeys(chain))
        if (len(chain) == 8):
          chain = chain[-i:] + chain[:-i]
          avoidDuplicatesAppend(chain, chains)
  return chains

# Appends element to list if element is not already in list
def avoidDuplicatesAppend(element, elementList: list):
  for l in elementList:
    if(l == element):
      return
  elementList.append(element)

# Finds indexes of all chains containing word
def lookupChain(word, chains):
  acc = []
  for i, chain in enumerate(chains):
    for link in chain:
      if (word == link):
        acc.append(i)
  return acc

# Returns true if there exists a qword chain that matches candidate
def validQchainCheck(candidate, chainsQ):
  for chain in chainsQ:
    if(chain == candidate):
      return True
  return False

# Finds hit in map that starts with 'dword'
def lookupQwordInMapFirstDword(dword, maps):
  candidate = ""
  bestHit = 0
  for map in maps:
    for hit in map:
      if(hit[0][:8] == dword and int(hit[1]) > bestHit):
        candidate = hit[0][:16]
        bestHit = int(hit[1])
  return candidate

# Finds hit in map where second double word is 'dword'
def lookupQwordInMapSecondDword(dword, maps):
  candidate = ""
  bestHit = 0
  for map in maps:
    for hit in map:
      if(hit[0][8:16] == dword and int(hit[1]) > bestHit):
        candidate = hit[0][:16]
        bestHit = int(hit[1])
  return candidate


# Finds all quad words that 'dword' appears in, to find the paired double word
# Then finds and returns every dword chain the paired double word appears in.  
def findPairedDwordChains(halfChain, chainsQ, chainsD):
  acc = []
  for Qchain in chainsQ:
    Qchain[0]
    if(Qchain[0][:8] == halfChain[0]):
      indexes = lookupChain(Qchain[0][8:], chainsD)
      for i in indexes:
        pairedChain = chainsD[i]
        dwordPair0 = halfChain[0] + pairedChain[1]
        dwordPair2 = halfChain[2] + pairedChain[5]
        if(dwordPair0 == Qchain[0] and dwordPair2 == Qchain[2]):
          avoidDuplicatesAppend(chainsD[i], acc)
    if(Qchain[0][8:] == halfChain[0]):
      indexes = lookupChain(Qchain[0][:8], chainsD)
      for i in indexes:
        pairedChain = chainsD[i]
        dwordPair0 = pairedChain[0] + halfChain[0]
        dwordPair2 = pairedChain[4] + halfChain[2]
        if(dwordPair0 == Qchain[0] and dwordPair2 == Qchain[2] ):
          avoidDuplicatesAppend(pairedChain, acc)
  return acc

# Combines chains to assemble blocks of 6 ymm registers each
def findBlocks(chainsQ, chainsD, mapsQ):
  blocks = []
  # Loops through all qword chains and tests if the first qword could be from ymm4
  for y4QChain in chainsQ:
      y4d1 = y4QChain[0][:8]
      y4d2 = y4QChain[0][8:]

      numberOfY4EvenChains = len(lookupChain(y4d2, chainsD))
      # If there exists any even dword chains, then this can not be ymm4
      if(numberOfY4EvenChains > 0):
        continue
      y4DChainCandidates = lookupChain(y4d1, chainsD)
      for y4DChainIndex in y4DChainCandidates:
        y4DChain = chainsD[y4DChainIndex]
        
        y4q1 = y4QChain[0]
        y4q2 = lookupQwordInMapFirstDword(y4DChain[2], mapsQ)
        y4q3 = y4QChain[2]
        y4q4 = lookupQwordInMapFirstDword(y4DChain[6], mapsQ)
        ymm4 = [y4q1, y4q2, y4q3, y4q4]
        if("" in ymm4):
          continue
        # Looks for the qword containing (x,y1d2) and returns all chains containing x
        y1DChainCandidates = findPairedDwordChains([y4DChain[1], y4DChain[3], y4DChain[5], y4DChain[7]], chainsQ, chainsD)
        for y1DChain in y1DChainCandidates:
          # Validates that Yellow Qword Chain exists in ChainsQ
          if(not validQchainCheck([y4q1, y1DChain[2]+y4DChain[3], y4q3, y1DChain[6]+y4DChain[7]], chainsQ)):
            continue
          y5d2 = y1DChain[1]
          # Looks for the qword containing (x,y5d2) and returns all chains containing x
          y5DChainCandidates = findPairedDwordChains([y1DChain[1], y1DChain[3], y1DChain[5], y1DChain[7]], chainsQ, chainsD)
          for y5DChain in y5DChainCandidates:
            # Validates that Blue Qword Chain exists in ChainsQ
            if(not validQchainCheck([y1DChain[0] + y4DChain[1], y5DChain[2] + y1DChain[3], y1DChain[4] + y4DChain[5], y5DChain[6] + y1DChain[7]], chainsQ)):
               continue
            y2d2 = y5DChain[1]
            # Looks for the qword containing (x,y2d2) and returns all chains containing x
            y2DChainCandidates = findPairedDwordChains([y5DChain[1], y5DChain[3], y5DChain[5], y5DChain[7]], chainsQ, chainsD)
            for y2DChain in y2DChainCandidates:
              # Validates that Red Qword Chain exists in ChainsQ
              if(not validQchainCheck([y5DChain[0] + y1DChain[1], y2DChain[2] + y5DChain[3], y5DChain[4] + y1DChain[5], y2DChain[6] + y5DChain[7]], chainsQ)):
                continue
              y6d2 = y2DChain[1]
              # Looks for the qword containing (x,y6d2) and returns all chains containing x
              y6DChainCandidates = findPairedDwordChains([y2DChain[1], y2DChain[3], y2DChain[5], y2DChain[7]], chainsQ, chainsD)
              for y6DChain in y6DChainCandidates:
                # Validates that Green Qword Chain exists in ChainsQ
                if(not validQchainCheck([y2DChain[0] + y5DChain[1], y6DChain[2] + y2DChain[3], y2DChain[4] + y5DChain[5], y6DChain[6] + y2DChain[7]], chainsQ)):
                  continue
                y3d2 = y6DChain[1]
                # Looks for the qword containing (x,y3d2) and returns all chains containing x
                y3DChainCandidates = findPairedDwordChains([y6DChain[1], y6DChain[3], y6DChain[5], y6DChain[7]], chainsQ, chainsD)
                # If there exists any odd dword chains, then this can not be ymm3
                if(len(y3DChainCandidates) < 1):
                  # Reconstruct the registers, each register being saved as a list of 4 qwords

                  ymm1 = [y1DChain[0] + y4DChain[1], y1DChain[2] + y4DChain[3], y1DChain[4] + y4DChain[5], y1DChain[6] + y4DChain[7]]
                  ymm2 = [y2DChain[0] + y5DChain[1], y2DChain[2] + y5DChain[3], y2DChain[4] + y5DChain[5], y2DChain[6] + y5DChain[7]]
                  ymm5 = [y5DChain[0] + y1DChain[1], y5DChain[2] + y1DChain[3], y5DChain[4] + y1DChain[5], y5DChain[6] + y1DChain[7]]
                  ymm6 = [y6DChain[0] + y2DChain[1], y6DChain[2] + y2DChain[3], y6DChain[4] + y2DChain[5], y6DChain[6] + y2DChain[7]]
                  # Finds missing segments of ymm3 and ymm4 by looking the found dwords up in the hit-map
                  y3q1 = lookupQwordInMapSecondDword(y6DChain[1], mapsQ)
                  y3q2 = lookupQwordInMapSecondDword(y6DChain[3], mapsQ)
                  y3q3 = lookupQwordInMapSecondDword(y6DChain[5], mapsQ)
                  y3q4 = lookupQwordInMapSecondDword(y6DChain[7], mapsQ)
                  
                  # Validates that Turquoise Qword Chain exists in ChainsQ
                  if(not validQchainCheck([y6DChain[0] + y2DChain[1], y3q2, y6DChain[4]+y2DChain[5], y3q4], chainsQ)):
                    continue

                  ymm3 = [y3q1, y3q2, y3q3, y3q4]
                  if("" in ymm3):
                    continue
                  avoidDuplicatesAppend([ymm1, ymm2, ymm3, ymm4, ymm5, ymm6], blocks)
  return blocks

def filterMaps(maps):
  for m in range(len(maps)):
    maps[m] = applyFilter(maps[m], sameBytesFilter)
    maps[m] = applyFilter(maps[m], inIntersectionFilter)    

def applyFilter(unfiltered, filterFunction):
  filterIterator = filter(filterFunction, unfiltered)
  filtered = []
  for i in filterIterator:
    filtered.append(i)
  return filtered

def sameBytesFilter(elem):
  qword = elem[0]
  max_count = 0
  for idx in range(0,len(qword), 2):
    byte = qword[idx:idx+2]
    count = qword.count(byte)
    if count > max_count:
      max_count = count
  return max_count < 4

def inIntersectionFilter(elem):
    dword0 = elem[0][:8]
    dword1 = elem[0][8:16]
    dword2 = elem[0][16:]
    for line in intersection:
      intersectionDwords = [line[:8], line[8:16], line[16:]]
      if(dword0 in intersectionDwords or  dword1 in intersectionDwords):
        return False
      if(len(dword2) == 8 and dword2 in intersectionDwords):
        return False
    return True

# Converts from a list of list of strings to a single string
def unpackBlocks(blocks):
  blockString = ""
  for b in blocks:
    for ymm in b:
      for qword in ymm:
        blockString += qword
  return blockString

# Groups blocks into groups depending on first qword in block
def groupBlocks(blocks):
  groups = []
  for b in blocks:
    for i in range(len(groups)):
      if(groups[i][0][0][0] == b[0][0]):
        groups[i].append(b)
        break
    else:
      groups.append([b])
  return groups


def writePublicKeyBytesToFile(pk):
  file = open("ProjectCode/KyberAttack/public_key.txt", "wb+")
  pkBytes = bytes.fromhex(pk)
  file.write(pkBytes)
  file.close()
  return

def identifyCorrectBlocks(blockList, sk, k):
  skChunks = []
  for i in range(k*2):
    skChunks.append(sk[i*(384):(i+1)*(384)])
  for c in skChunks:
    for i, b in enumerate(blockList):
      blockString = ""
      for ymm in b:
        for qword in ymm:
          blockString += qword
      if(blockString == c):
        print(f"Block {i} was a match")


def reconstructKey(): 
  fD = open("ProjectCode/KyberAttack/mapD.txt", "r") 
  fQ = open("ProjectCode/KyberAttack/mapQ.txt", "r") 
  
  fKey = open("ProjectCode/KyberAttack/output.txt")
  lines = fKey.readlines()
  pk = lines[0].split(" ")[1].strip()
  sk = lines[1].split(" ")[1].strip()
  fKey.close()
  # Determine the value of KYBER_K from length of public key.
  # Determines how many blocks in the secret key
  k = (len(pk)-64) // 768

  mapsD = getMapsFromFile(fD)
  mapsQ = getMapsFromFile(fQ)
  filterMaps(mapsD)
  filterMaps(mapsQ)
  chainsD = findChainsD(mapsD)
  print(f"found {len(chainsD)} chainsD")
  chainsQ = findChainsQ(mapsQ)
  print(f"found {len(chainsQ)} chainsQ")

  blocks = findBlocks(chainsQ, chainsD, mapsQ)
  for i, block in enumerate(blocks):
    print("")
    print(f"Block {i}:")
    for chain in block:
      print(chain)

  groups = groupBlocks(blocks)
  for i, g in enumerate(groups):
    print("")
    print("Group",i)
    for b in g:
      for l in b:
        print(l)
      
  print(f"{len(blocks)} blocks found, in {len(groups)} groups")


  writePublicKeyBytesToFile(pk)
  identifyCorrectBlocks(blocks, sk, k)


  f = open("ProjectCode/KyberAttack/possible_secret_keys.txt", "wb+")
  for permutationOrder in itertools.permutations(groups, k*2):
      for permutation in itertools.product(*permutationOrder):
        candidate = unpackBlocks(permutation)
        if candidate == sk: 
          print("Found secret key")
        candidateBytes = bytes.fromhex(candidate)
        f.write(candidateBytes)
  f.close()

reconstructKey()