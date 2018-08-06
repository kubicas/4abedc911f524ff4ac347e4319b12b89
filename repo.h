#ifndef INTF_REPO__REPO_H
#define INTF_REPO__REPO_H

#include <memory>
#include <iosfwd>
#include <string>
#include <vector>

namespace repo
{

struct repo_ref_t
{
    char const* m_remote_name; // e.g. "libgit2/libgit2" for "git@github.com:libgit2/libgit2.git" or "https://github.com/libgit2/libgit2.git"
    char const* m_local_name; // e.g. "libgit2" for "git@github.com:libgit2/libgit2.git" or "https://github.com/libgit2/libgit2.git"
    virtual ~repo_ref_t();
protected:
    repo_ref_t()
        : m_remote_name(nullptr)
        , m_local_name(nullptr)
    {}
};
inline repo_ref_t::~repo_ref_t() {};

struct git_repo_ref_t
    : repo_ref_t
{
    char const* m_host; // e.g. github.com, tcp-ip address of local server
    char const* m_subdir; // e.g. "libgit2/" for "https://github.com/libgit2/libgit2.git"
    char const* m_extension; // extension of archive name; when nullptr, uses ".git"
    char const* m_branch; // branch to update to; when nullptr, update to master
    char const* m_commit_sha; // commit sha to update to; when nullptr, update to head
    char const* m_commit_user; // commit user name
protected:
    git_repo_ref_t()
        : m_host(nullptr)
        , m_subdir(nullptr)
        , m_extension(nullptr)
        , m_branch(nullptr)
        , m_commit_sha(nullptr)
        , m_commit_user(nullptr)
    {}
};

struct githttps_repo_ref_t
    : git_repo_ref_t
{
};

struct gitfile_repo_ref_t
    : git_repo_ref_t
{
};

struct gitssh_repo_ref_t
    : git_repo_ref_t
{
    gitssh_repo_ref_t()
        : m_gituser(nullptr)
    {}
    char const* m_gituser; // ssh user name; when nullptr, user "git" is used
};

/* get(...)
1) When no archive with dirname exists, then the archive referenced in repo_ref will be cloned
2) local archive will be updated, including all sub modules
3) For git: when m_commit_user is supplied, the user name will be configured in the archive
4) For git: when m_commit_email is supplied, the user e-mail will be configured in the archive

Upon failure get(...) will throw a standard exception.
Common failures:
- repo_ref_t passed -> std::logic_error
- repo_ref_t::m_name == nullptr -> std::logic_error
- git..._repo_ref_t::m_host == nullptr -> std::logic_error
- git..._repo_ref_t::m_commit_user == nullptr -> std::logic_error
- git..._repo_ref_t::m_commit_email == nullptr -> std::logic_error
- path == nullptr -> std::logic_error
- path does not end with "/projects/" -> std::runtime_error
- dirname empty string -> std::runtime_error
- archive cloning fails -> std::runtime_error
- archive already exists, but no overwrite specified -> std::runtime_error or callback??
- update fails -> std::runtime_error
*/

struct repo_t
{
public:
    virtual ~repo_t() = 0;
    virtual void get(
        repo_ref_t const& repo_ref,
        char const* path, // path to projects directory; this path MUST end with "/projects/"
        char const* dirname) = 0; // directory name; when nullptr, use default
    virtual bool has_commit_user() = 0;
};
inline repo_t::~repo_t() {}

typedef void(*ask_user_pwd_t)(std::ostream& os, std::istream& is, std::string& user, std::string& pass, char const *url);
std::unique_ptr<repo_t> create_repo(
    std::ostream& os, // supply output stream (or null_stream)
    std::istream& is, // supply input stream, only used to be passed with ask_user_pwd_t
    ask_user_pwd_t ask_pwd_user); // call back to ask for password; when nullptr -> no password can be asked

enum class host_type_t
{
    https,
    file,
    ssh
};

struct repository_t
{
    char const* m_local;
    char const* m_remote;
    host_type_t m_host_type;
    char const* m_host;
    char const* m_subdir;
};

void flying_start(std::vector<repository_t> const& repositories, int argc, char const* argv[]);

#define REPO_ARCHIVE_USB 1
#define REPO_ARCHIVE_GITHUB_HTTPS 2
#define REPO_ARCHIVE_TYPE REPO_ARCHIVE_GITHUB_HTTPS // change this line to swith to other archive type

// don't change code below
#if REPO_ARCHIVE_TYPE == REPO_ARCHIVE_USB
host_type_t const archive_host_type = host_type_t::file;
char const*const archive_host       = "../procts_repo";
char const*const archive_subdir     = "git/";
#elif REPO_ARCHIVE_TYPE == REPO_ARCHIVE_GITHUB_HTTPS
host_type_t const archive_host_type = host_type_t::https;
char const*const archive_host       = "github.com";
char const*const archive_subdir     = "kubicas/";
#endif

}; // namespace repo

#endif // INTF_REPO__REPO_H
