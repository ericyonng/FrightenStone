/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : TestDaemon.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/07/18
 * @brief :
 */
#ifndef __Test_TestDaemon_H__
#define __Test_TestDaemon_H__

#pragma once

#define BD_MAX_CLOSE 8192

class TestDaemon
{
public:
    static void Run()
    {
#ifndef _WIN32
        // 避免僵尸进程
        signal(SIGCHLD, SIG_IGN);
        // 杀死会话组组长时进程组所有进程都会收到该信号,该信号默认处理是终止进程
        signal(SIGHUP, SIG_IGN);
        if (fork() != 0) {
            exit(0);
        }
        setsid();
        if (fork() != 0) {
            exit(0);
        }

        umask(0);
        chdir("/");

        int maxFd = sysconf(_SC_OPEN_MAX);
        if (maxFd == -1)
            maxFd = BD_MAX_CLOSE;

        for (int i = 0; i < maxFd; ++i)
            close(i);

        close(STDIN_FILENO);
        int fd = open("/dev/null", O_RDWR);
        if (fd != STDIN_FILENO)
            exit(0);
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            exit(0);
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            exit(0);

        while (true)
        {
            sleep(1);
        }
#endif
    }
};
#endif
