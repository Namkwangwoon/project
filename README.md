# Make virtual linux file system and use own shell

### how to excute

```bash
  gcc mymkfs.c -o mymkfs
  gcc myfs_shell.c -o myshell
  ./mymkfs # make virtual shell
  ./myshell # excute shell
```

### ininital shell

![operation](./images/initial.png)

### Operation list

- default bash script, if operation does not start with "my" excute default bash script
  ![operation](./images/default.png)
- mycpfrom: copy local file to virtual my files ystem.
- myls: show list virtual filesystem.
- mycat: show file's content.
  ![operation](./images/mycpfrom.png)
- mystate: show virtual system's info
  ![operation](./images/mystate.png)
- myshowfile: show file's start byte ~ end byte
  ![operation](./images/myshowfile.png)
- mymkdir: make directory
- mycd: change directory
- mypwd: show current directory
  ![operation](./images/mymkdir.png)
- myshowinode: show inode's detail
  ![operation](./images/myshowinode.png)
- myshowblock: show block's detail, if block is directory, show list file names, or block is regular file, show file's content
  ![operation](./images/myshowblock.png)
- mytouch: create empty file
  ![operation](./images/mytouch.png)
- mycpto: copy virtual file system's file to local file system
  ![operation](./images/mycpto.png)
- mycp: copy file
  ![operation](./images/mycp.png)
- mymv: move or rename file
  ![operation](./images/mymv.png)
- myrm: remove file
  ![operation](./images/myrm.png)
- myrmdir: remove directory
  ![operation](./images/myrmdir.png)
- mytree: show tree structure virtual file system
  ![operation](./images/mytree.png)
- exit, byebye: exit shell
  ![operation](./images/exit.png)
