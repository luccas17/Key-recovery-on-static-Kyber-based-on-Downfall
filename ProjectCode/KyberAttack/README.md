### KyberAttack 
This foolder contains compiled versions of the two attackers, and of the 3 victims, aswell as a the python script for generating candidate keys, and the c script for checking these keys using the known public key. 

#### Checking leaks 
The mapD, mapQ and output files contain a run of the attack. Currently, they contain a successful run for K=3, running for 35 minutes. 
To make the candidate keys, run ```python3 ReconstructKey.py```, and then afterwards run ```test_possible_keys'x'``` to check if the secretkey was found. 
The python script also prints some information about found blocks, and uses the stored secret-key to check if the reconstruction was a success. 
The c script only uses candidate keys from the python scripts output to check using the public key. 

#### Running the attack 
The attack can be run by either running victim2, victim3, or victim4 as the victim. Afterwards, attackD.sh and attackQ.sh can be run. 
We chose to run the commands directly in the command prompt instead of using the shell scripts, as this gave better performance. 

#### Data
Some runs for various security settings and times. Since the python script loads "mapQ.txt", "mapD.txt" and "output.txt" by default, any triple from the data section can simply be copied out into the KyberAttack folder and renamed, and the processing can be run anew. 
The output files contain both the secret key and the public key, which can be used for varification. 

### GDSQ and GDSD
These contain the final code for the two attackers used. This code is much alike that of AllTests and 8BytesGatherQ, but with the changes seen in section 4.5. GDSQ uses vpgatherqq and permq, and GDSD uses vpgatherdd and vpermd. 

