/home/patelvrajn/qemu/qemu-9.0.4/bin/debug/native/qemu-system-x86_64 \
-drive file=cosmos.img,unit=0,format=raw \
-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
-drive if=pflash,unit=1,format=raw,file=ovmf/ovmf-vars-x86_64.fd \
-display gtk \
-m 2048M \
-d in_asm,cpu_reset,op,int \
-D qemu_log.log \
-chardev stdio,mux=on,id=char0,logfile=stdio.log,signal=off \
-mon chardev=char0 \
-serial chardev:char0 \
-monitor telnet:127.0.0.1:9532,server,nowait # \
# -s \
# -S 
