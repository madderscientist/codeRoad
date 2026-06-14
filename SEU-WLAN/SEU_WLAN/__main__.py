from . import login, logout

if __name__ == "__main__":
    """
    # 登录（需要 -a 和 -p）
    python main.py login -a 213210137 -p 123456

    # 登出（只需要 -a）
    python main.py logout -a 213210137

    # 查看帮助
    python main.py -h
    python main.py login -h
    python main.py logout -h
    """
    import argparse
    import sys
    parser = argparse.ArgumentParser(description='SEU校园网工具')
    subparsers = parser.add_subparsers(dest='command', help='可用命令')

    parser_login = subparsers.add_parser('login', help='登录校园网')
    parser_login.add_argument('-a', '--account', required=True, help='账号')
    parser_login.add_argument('-p', '--password', required=True, help='密码')

    parser_logout = subparsers.add_parser('logout', help='登出校园网')
    parser_logout.add_argument('-a', '--account', required=True, help='账号')
    
    args = parser.parse_args()
    if args.command == 'login':
        result = login(args.account, args.password)
        if result is not None:
            print(result)
            if result["result"] == "1":
                sys.exit(0)
    elif args.command == 'logout':
        result = logout(args.account)
        if result is not None:
            print(result)
            if result["result"] == "1":
                sys.exit(0)
    else:
        parser.print_help()
    sys.exit(1)