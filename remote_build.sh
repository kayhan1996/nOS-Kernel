#!/bin/zsh

HOST=kay@192.168.1.100
PORT=15429

GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}Copying Sources${NC}"
ssh -p ${PORT} ${HOST} "fuser -k 9000/tcp; cd ~; mkdir -p ~/tmp"
rsync -e "ssh -p ${PORT}" -rzzu ./src ${HOST}:~/tmp/
rsync -e "ssh -p ${PORT}" -rzzu ./include ${HOST}:~/tmp/
rsync -e "ssh -p ${PORT}" -zzu ./Makefile ${HOST}:~/tmp/
rsync -e "ssh -p ${PORT}" -zzu ./linker.ld ${HOST}:~/tmp/

echo -e "${GREEN}Starting Build${NC}"
ssh -p ${PORT} ${HOST} "cd ~/tmp; make"

mkdir -p ./build
rsync -e "ssh -p ${PORT}" -zzu ${HOST}:~/tmp/build/kernel.sym ./build/kernel.sym
echo -e "${GREEN}Build Complete${NC}\n"

ssh -p ${PORT} ${HOST} "cd ~/tmp; make run"