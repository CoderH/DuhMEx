# DuhMEx
自用mfc类汇总，提供vc6、vs2013版本dll
## AES.h
AES加密、解密。
* 支持以16进制可读字符串编码结果密文
* 支持以Base64编码结果密文
## SearchEdit.h
带下拉框的文本编辑框。
## DetailWnd.h
提示窗
## SkinMenuMgr.h
利用钩子子类化程序弹出式菜单，重新绘制样式。  
只支持有发送WM_INITMENUPOPUP消息的窗口弹出的菜单（不支持cedit控件菜单）
#### 使用
``` C++
Duh::CSkinMenuMgr::Instance()->GetStyle(); //获取样式进行修改
Duh::CSkinMenuMgr::Instance()->Hook();     //下钩子
Duh::CSkinMenuMgr::Instance()->UnHook();   //取消钩子
```
