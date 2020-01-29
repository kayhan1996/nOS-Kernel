#!/bin/bash

HOST="kay@192.168.1.100"
PORT=15429

GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}Copying Source${NC}"
ssh -p ${PORT} ${HOST} "fuser -k 9000/tcp; cd ~; mkdir -p tmp"
rsync 'ssh -p 15429' -rzu --stats ../nOS-Kernel/src ${HOST}:~/tmp/ | grep "files transferred"
rsync 'ssh -p ${PORT}' -rzu ../nOS-Kernel/include ${HOST}:~/tmp/
rsync 'ssh -p ${PORT}' -zu ../nOS-Kernel/Makefile ${HOST}:~/tmp/
rsync 'ssh -p ${PORT}' -zu ../nOS-Kernel/linker.ld ${HOST}:~/tmp/

echo -e "${GREEN}Starting Build${NC}"
ssh -p ${PORT} ${HOST} "cd ~/tmp; make"

rsync -P ${PORT} ${HOST}:~/tmp/build/kernel.sym kernel.sym
echo -e "${GREEN}Build Complete${NC}\n"

ssh -p ${PORT} ${HOST} "cd ~/tmp; make run"