#include "FileUtil.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace generic;

FileUtil::FileUtil(std::string name, int maxsize = DEFAULT_FILE_SIZE) : m_fd(0),
                                                                        m_name(name),
                                                                        m_writtenBytes(0),
                                                                        MAX_SIZE(maxsize)
{
}

FileUtil::~FileUtil()
{
    close();
}

void FileUtil::open()
{
    open_internal(O_RDWR | O_CREAT); // | O_APPEND
}

inline void FileUtil::open_internal(int flags)
{
    int fd = ::open(m_name.c_str(), flags, 0666);
    if (fd < 0)
    {

        perror(m_name.c_str());
        abort();
    }
    m_fd = fd;
}

void FileUtil::close()
{
    if (m_fd < 0)
        return;
    if (::close(m_fd) < 0)
    {
        perror(m_name.c_str());
        abort();
    }
    m_fd = -1;
}

int FileUtil::write(const char *data, int len)
{
    int remain = MAX_SIZE - m_writtenBytes;
    if (remain < len)
        return -1;

    ssize_t n = ::write(m_fd, data, len);
    if (n < 0)
    {
        ::close(m_fd);
        perror(m_name.c_str());
        abort();
    }
    m_writtenBytes += n;
    return n;
}

int FileUtil::read(char *buf, long maxlen) const
{
    ssize_t n = ::read(m_fd, buf, maxlen);
    if (n < 0)
    {
        ::close(m_fd);
        perror(m_name.c_str());
        abort();
    }
    return n;
}

bool FileUtil::exist() const
{
    int ret = access(m_name.c_str(), R_OK | W_OK);
    if (ret < 0)
    {
        if (errno == ENOENT)
        {
            return false;
        }
        else
        {
            perror(m_name.c_str());
            abort();
        }
    }

    return true;
}

int Console::write(const char *buf, int len)
{
    int n = ::write(STDOUT_FILENO, buf, len);
    if (n < 0)
    {
        perror("STDOUT_FILENO");
        abort();
    }
    return n;
}

int Console::read(char *buf, int bytes)
{
    int n = ::read(STDIN_FILENO, buf, bytes);
    if (n < 0)
    {
        perror("STDIN_FILENO");
        abort();
    }
    return n;
}