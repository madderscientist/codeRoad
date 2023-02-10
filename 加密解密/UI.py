import tkinter
from tkinter import messagebox,filedialog
import Cryptology as cry
import pickle

# 出现在中心
def midshow(who, w, h):
    WW = who.winfo_screenwidth()
    HH = who.winfo_screenheight()
    x = (WW - w) / 2
    y = (HH - h) / 2
    size_xy = '%dx%d+%d+%d' % (w, h, x, y)
    who.geometry(size_xy)
    who.resizable(False, False)


def newface(tour):
    # tour为游客登录，true为游客登录，无默认秘钥
    root.destroy()
    root2 = tkinter.Tk()
    root2.title('密!')
    midshow(root2, 190, 115)

    def fileSelect():
        global filename
        filename = filedialog.askopenfilename()
        if filename != '':
            filename=filename.replace('/','\\')
            chosfile.config(text=filename)
        else:
            chosfile.config(text="还没选择文件")

    def ifsetkey():
        global ifset
        if setkey.get()!='':
            # 设置了自定义密钥
            cry.HSKL(setkey.get(),True)
            ifset=True
        else:
            if tour:
                # 没有设置且为游客登录
                messagebox.showerror('Error', '游客必须设置密钥！')
                return True
            else:
                # 为账号登录
                if ifset:
                    # 没有设置且上次设置了，要重置密钥
                    cry.HSKL(globalname+globalpwd,True)
                    ifset=False
        return False

    def encryption():
        if filename == '':
            messagebox.showerror('Error', '还没选择要加密的文件！')
            return

        if ifsetkey():
            return

        temp1=filename.rfind('\\')+1
        temp2=filename.rfind('.')
        keyname=filename[temp1:temp2 if temp2 != -1 else None]
        newname = filedialog.asksaveasfilename(title='保存文件',filetypes=(("Locked Files", "*.lock"), ("All Files", "*.*")),initialdir='./',initialfile=keyname+'.lock')
        if newname=='':
            return
        if cry.EncryptUTF(filename,newname.replace('/','\\')):
            cry.fileDeleteUTF(filename)
            messagebox.showinfo('Success', '加密成功！')
        else:
            messagebox.showerror('Error', '加密失败！')

    def decrypt():
        if filename == '':
            messagebox.showerror('Error', '还没选择要解密的文件！')
            return

        if ifsetkey():
            return

        newname = filedialog.askdirectory(title='选择解密到的文件夹',initialdir='./')
        if newname=='':
            return
        if cry.DecipherUTF(filename,newname.replace('/','\\')+'\\'):
            cry.fileDeleteUTF(filename)
            messagebox.showinfo('Success', '解密成功！')
        else:
            messagebox.showerror('Error', '解密失败！')

    chosfile = tkinter.Button(root2, text="选择文件", command=fileSelect)
    chosfile.place(relx=0.05, rely=0, relheight=0.25, relwidth=0.9)
    tkinter.Label(root2,text='自定义密钥，'+('游客必填' if tour else '不填则用账户密钥')).place(relx=0,rely=0.25,relheight=0.21,relwidth=1)
    setkey = tkinter.Entry(root2)
    setkey.place(relx=0.05,rely=0.46,relheight=0.21,relwidth=0.9)
    tkinter.Button(root2, text='加密', command=encryption).place(relx=0.05,rely=0.71,relwidth=0.45,relheight=0.25)
    tkinter.Button(root2, text='解密', command=decrypt).place(relx=0.5,rely=0.71,relwidth=0.45,relheight=0.25)
    root2.mainloop()

ifset=False
root = tkinter.Tk()
root.title('')
midshow(root, 190, 115)
username = tkinter.Entry(root)
password = tkinter.Entry(root,show='*')


def touristin():
    newface(True)


def LOGIN():
    global globalname
    global globalpwd
    globalname = username.get()
    globalpwd = password.get()
    hashcode=cry.HSKL(globalname+globalpwd,True)

    # 这里设置异常捕获，第一次访问用户信息文件时是不存在的
    try:
        with open('user_info.dat', 'rb') as usr_file:
            usrs_info = pickle.load(usr_file)
    except FileNotFoundError:
        is_sign_up = messagebox.askyesno('Welcome！', '您还未注册。是否现在注册？')
        # 提示需不需要注册新用户
        if is_sign_up:
            SIGNUP(globalname,globalpwd)
            return

    # 如果用户名和密码与文件中的匹配成功，则打开加密界面。
    if globalname in usrs_info:
        if hashcode == usrs_info[globalname]:
            newface(False)
        # 如果用户名匹配成功，而密码输入错误
        else:
            messagebox.showerror(message='密码错误！')
    else:  # 如果用户名不存在
        is_sign_up = messagebox.askyesno('Welcome', '您还未注册。是否现在注册？')
        # 提示需不需要注册新用户
        if is_sign_up:
            SIGNUP(globalname,globalpwd)


def SIGNUP(n='',p=''):
    def sign_to():
        np = new_pwd.get()
        npf = new_pwd_confirm.get()
        nn = new_name.get()

        if nn=='':
            messagebox.showerror('Error', '用户名不能为空！')
            return
        elif np=='':
            messagebox.showerror('Error', '密码不能为空！')
            return

        # 打开数据文件，第一次访问没有数据文件，捕获异常
        try:
            with open('user_info.dat', 'rb') as usr_file:
                exist_usr_info = pickle.load(usr_file)
        except FileNotFoundError:
            with open('user_info.dat', 'wb') as usr_file:
                exist_usr_info = {}
                pickle.dump(exist_usr_info, usr_file)
                usr_file.close()    # 必须先关闭，否则pickle.load()会出现EOFError: Ran out of input

        # 如果两次密码输入不一致
        if np != npf:
            messagebox.showerror('Error', '两次密码不一样！')

        # 如果用户名已经在我们的数据文件中
        elif nn in exist_usr_info:
            messagebox.showerror('Error', '该用户名已经被注册了！')

        # 最后如果输入无以上错误，则将注册输入的信息记录到文件当中，并提示注册成功，然后销毁窗口。
        else:
            exist_usr_info[nn] = cry.HSKL(nn+np,True)
            with open('user_info.dat', 'wb') as usr_file:
                pickle.dump(exist_usr_info, usr_file)
            messagebox.showinfo('Welcome', '注册成功！')
            window_sign_up.destroy()

    # 定义长在窗口上的窗口
    window_sign_up = tkinter.Toplevel(root)
    window_sign_up.title('注册')
    window_sign_up.geometry('200x100')  # 看得清楚，不设在中间

    new_name = tkinter.StringVar()
    if n!='':
        new_name.set(n)
    tkinter.Label(window_sign_up, text='用户名').place(x=0, y=0,relheight=0.25,relwidth=0.3)
    entry_new_name = tkinter.Entry(window_sign_up, textvariable=new_name)
    entry_new_name.place(relx=0.3,rely=0.02,relheight=0.21,relwidth=0.65)

    new_pwd = tkinter.StringVar()
    if p!='':
        new_pwd.set(p)
    tkinter.Label(window_sign_up, text='密码').place(x=0,rely=0.25,relheight=0.25,relwidth=0.3)
    entry_usr_pwd = tkinter.Entry(window_sign_up, textvariable=new_pwd, show='*')
    entry_usr_pwd.place(relx=0.3,rely=0.27,relheight=0.21,relwidth=0.65)

    new_pwd_confirm = tkinter.StringVar()
    tkinter.Label(window_sign_up, text='确认密码').place(x=0,rely=0.5,relheight=0.25,relwidth=0.3)
    entry_usr_pwd_confirm = tkinter.Entry(window_sign_up, textvariable=new_pwd_confirm, show='*')
    entry_usr_pwd_confirm.place(relx=0.3,rely=0.52,relheight=0.21,relwidth=0.65)

    btn_comfirm_sign_up = tkinter.Button(window_sign_up, text='注册!', command=sign_to)
    btn_comfirm_sign_up.place(relx=0.1,rely=0.75,relheight=0.25,relwidth=0.8)


tkinter.Label(root, text='用户名').place(x=0,rely=0,relheight=0.25,relwidth=0.25)
username.place(relx=0.25, rely=0, relheight=0.25, relwidth=0.7)
tkinter.Label(root, text='密码').place(x=0,rely=0.25,relheight=0.25,relwidth=0.25)
password.place(relx=0.25, rely=0.25, relheight=0.25, relwidth=0.7)
tkinter.Button(root, text='登录', command=LOGIN).place(relx=0.05,rely=0.5,relheight=0.25,relwidth=0.45)
tkinter.Button(root, text='注册', command=SIGNUP).place(relx=0.5,rely=0.5,relheight=0.25,relwidth=0.45)
tkinter.Button(root, text='跳过登录', command=touristin).place(relx=0.05,rely=0.75,relwidth=0.9,relheight=0.25)
root.mainloop()