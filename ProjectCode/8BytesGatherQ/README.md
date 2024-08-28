### 8BytesGatgerQ
A simple version of one of our Downfall experiments. Here, the vicitm loads 1 ymm register on repeat, and the attacker attempts to leak 8 bytes of data. 
Can be run by starting a victim using ```make vic && timeout -s2 123123 taskset -c 0 ./victim 0```, and then running the attacker using make ```clean && make && taskset -c 1 timeout -s2 30 ./attack```.