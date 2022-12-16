from pwn import *

r=remote('inp111.zoolab.org',10008);
def main():
    prefix=r.recvline().decode().split('\'')[1]
    print(prefix)
    for i in range(1000000000000):
        h=hashlib.sha1((prefix+str(i)).encode()).hexdigest();
        if(h[:6]=='000000'):
            ans=str(i).encode()
            print(h,i)
            break;
    
    print(time.time())

    r.sendlineafter(b'string S: ',base64.b64encode(ans));

if __name__=='__main__':
    main()
    r.interactive()
    r.close()
