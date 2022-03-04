// 将以root身份启动的进程切换为以一个普通用户身份运行
static bool switch_to_user( uid_t user_id, gid_t gp_id )
{
    /*先确保目标用户不是root*/
    if ( ( user_id == 0 ) && ( gp_id == 0 ) )
    {
        return false;
    }

    /*确保当前用户是合法用户：root或者目标用户*/
    gid_t gid = getgid();
    uid_t uid = getuid();
    if ( ( ( gid != 0 ) || ( uid != 0 ) ) && ( ( gid != gp_id ) || ( uid != user_id ) ) )
    {
        return false;
    }

    /*如果不是root，则已经是目标用户*/
    if ( uid != 0 )
    {
        return true;
    }

    /*切换到目标用户*/
    if ( ( setgid( gp_id ) < 0 ) || ( setuid( user_id ) < 0 ) )
    {
        return false;
    }

    return true;
}


// uid_t getuid();/*获取真实用户ID*/
// uid_t geteuid();/*获取有效用户ID*/
// gid_t getgid();/*获取真实组ID*/
// gid_t getegid();/*获取有效组ID*/
// int setuid(uid_t uid);/*设置真实用户ID*/
// int seteuid(uid_t uid);/*设置有效用户ID*/
// int setgid(gid_t gid);/*设置真实组ID*/
// int setegid(gid_t gid);/*设置有效组ID*/