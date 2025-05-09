import os
import errno
import threading
from time import time
from sys import argv, exit
from fusepy import FUSE, Operations, LoggingMixIn, FuseOSError

class GPTSession:
    def __init__(self):
        self.input = b""
        self.output = b""
        self.error = b""
        self.lock = threading.Lock()
        self.gpt_thread = None

    def run_gpt(self):
        # 模拟 GPT 回复
        try:
            prompt = self.input.decode('utf-8')
            reply = f"[模拟GPT回复] 你说的是: {prompt}"
            with self.lock:
                self.output = reply.encode("utf-8")
                self.error = b""
        except Exception as e:
            with self.lock:
                self.output = b""
                self.error = str(e).encode("utf-8")

    def trigger_gpt(self):
        if self.gpt_thread and self.gpt_thread.is_alive():
            return
        self.gpt_thread = threading.Thread(target=self.run_gpt)
        self.gpt_thread.start()

class GPTfs(LoggingMixIn, Operations):
    def __init__(self):
        self.sessions = {}  # session_name -> GPTSession
        self.fd = 0

    def getattr(self, path, fh=None):
        now = time()
        if path == '/':
            return dict(st_mode=(0o40755), st_nlink=2)
        parts = path.strip('/').split('/')
        if len(parts) == 1 and parts[0]:
            if parts[0] in self.sessions:
                return dict(st_mode=(0o40755), st_nlink=2)
            raise FuseOSError(errno.ENOENT)
        elif len(parts) == 2:
            session = parts[0]
            fname = parts[1]
            if session not in self.sessions:
                raise FuseOSError(errno.ENOENT)
            if fname in ("input", "output", "error"):
                size = self._get_size(session, fname)
                return dict(st_mode=(0o100644), st_nlink=1, st_size=size)
            raise FuseOSError(errno.ENOENT)
        else:
            raise FuseOSError(errno.ENOENT)

    def _get_size(self, session, fname):
        s = self.sessions[session]
        if fname == "input":
            return len(s.input)
        elif fname == "output":
            return len(s.output)
        elif fname == "error":
            return len(s.error)
        return 0

    def readdir(self, path, fh):
        if path == '/':
            return ['.', '..'] + list(self.sessions.keys())
        ps = path.strip('/').split('/')
        if len(ps) == 1 and ps[0] in self.sessions:
            return ['.', '..', 'input', 'output', 'error']
        raise FuseOSError(errno.ENOENT)

    def mkdir(self, path, mode):
        ps = path.strip('/').split('/')
        if len(ps) != 1:
            return -errno.EPERM
        name = ps[0]
        if name in self.sessions:
            return -errno.EEXIST
        self.sessions[name] = GPTSession()
        return 0

    def rmdir(self, path):
        ps = path.strip('/').split('/')
        if len(ps) != 1:
            return -errno.EPERM
        name = ps[0]
        if name not in self.sessions:
            raise FuseOSError(errno.ENOENT)
        del self.sessions[name]
        return 0

    def open(self, path, flags):
        ps = path.strip('/').split('/')
        if len(ps) != 2:
            return -errno.EPERM
        sess, fname = ps
        if sess not in self.sessions:
            raise FuseOSError(errno.ENOENT)
        if fname not in ('input', 'output', 'error'):
            raise FuseOSError(errno.ENOENT)
        self.fd += 1
        return self.fd

    def read(self, path, size, offset, fh):
        ps = path.strip('/').split('/')
        if len(ps) != 2:
            return b''
        sess, fname = ps
        s = self.sessions.get(sess)
        if not s:
            return b''
        with s.lock:
            buf = getattr(s, fname, b'')
            return buf[offset:offset + size]

    def write(self, path, data, offset, fh):
        ps = path.strip('/').split('/')
        if len(ps) != 2:
            return -errno.EPERM
        sess, fname = ps
        s = self.sessions.get(sess)
        if not s:
            raise FuseOSError(errno.ENOENT)
        if fname != 'input':
            return -errno.EPERM
        with s.lock:
            # 只支持 offset=0 覆盖写
            if offset == 0:
                s.input = data
                s.output = b""
                s.error = b""
            else:
                s.input += data
        s.trigger_gpt()
        return len(data)

    def truncate(self, path, length, fh=None):
        ps = path.strip('/').split('/')
        if len(ps) != 2:
            return -errno.EPERM
        sess, fname = ps
        s = self.sessions.get(sess)
        if not s:
            raise FuseOSError(errno.ENOENT)
        if fname == 'input':
            with s.lock:
                s.input = s.input[:length]
        return 0

    def unlink(self, path):
        return -errno.EPERM

if __name__ == '__main__':
    if len(argv) != 2:
        print('usage: {} <mountpoint>'.format(argv[0]))
        exit(1)
    mountpoint = argv[1]
    fuse = FUSE(GPTfs(), mountpoint, foreground=True, allow_other=True)