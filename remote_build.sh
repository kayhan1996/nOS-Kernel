#!/bin/bash

HOST="kay@192.168.1.96"

GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}Copying Source${NC}"
ssh ${HOST} "cd ~; mkdir -p tmp"
rsync -rzu --stats ../nOS-Kernel/src ${HOST}:~/tmp/ | grep "Number of files"
rsync -rzu ../nOS-Kernel/include ${HOST}:~/tmp/
rsync -zu ../nOS-Kernel/Makefile ${HOST}:~/tmp/
rsync -zu ../nOS-Kernel/linker.ld ${HOST}:~/tmp/

echo -e "${GREEN}Starting Build${NC}"
ssh ${HOST} "cd ~/tmp; make"

rsync ${HOST}:~/tmp/build/kernel.sym kernel.sym
echo -e "${GREEN}Build Complete${NC}\n"