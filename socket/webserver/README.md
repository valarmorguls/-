#web服务器
##http协议
"HTTP 协议"（HyperText Transfer Protocol，超文本传输协议）是因特网上应用最为广泛的一种
网络传输协议，所有的 WWW 文件都必须遵守这个标准。HTTP 是一个基于 TCP/IP 通信协议
来传递数据（HTML 文件, 图片文件, 查询结果等）。
###http简介
HTTP 协议是 Hyper Text Transfer Protocol（超文本传输协议）的缩写,是用于从万维网
（WWW:World Wide Web ）服务器传输超文本到本地浏览器的传送协议。HTTP 是一个基于
TCP/IP 通信协议来传递数据（HTML 文件, 图片文件, 查询结果等）。
##http工作原理
HTTP 协议工作于客户端-服务端架构上。浏览器作为 HTTP 客户端通过 URL 向 HTTP 服务端
即 WEB 服务器发送所有请求。
1.web服务器
    Apache服务器， IIS服务器（Internet Information Services）等。
2.Web 服务器根据接收到的请求后，向客户端发送响应信息。
3.HTTP 默认端口号为 80，但是你也可以改为 8080 或者其他端口。
HTTP三点注意事项
* HTTP 是无连接：无连接的含义是限制每次连接只处理一个请求。服务器处理完客户的
请求，并收到客户的应答后，即断开连接。采用这种方式可以节省传输时间。 
* HTTP 是媒体独立的：这意味着，只要客户端和服务器知道如何处理的数据内容，任何
类型的数据都可以通过 HTTP 发送。客户端以及服务器指定使用适合的 MIME-type 内容
类型。
* HTTP是无状态：HTTP协议是无状态协议。无状态是指协议对于事务处理没有记忆能力。
缺少状态意味着如果后续处理需要前面的信息，则它必须重传，这样可能导致每次连接
传送的数据量增大。另一方面，在服务器不需要先前信息时它的应答就较快。
##http消息结构
HTTP 是基于客户端/服务端（C/S）的架构模型，通过一个可靠的链接来交换信息，是一个无
状态的请求/响应协议。
一个 HTTP"客户端"是一个应用程序（Web 浏览器或其他任何客户端），通过连接到服务器达
到向服务器发送一个或多个 HTTP 的请求的目的。
一个 HTTP"服务器"同样也是一个应用程序（通常是一个 Web 服务，如 Apache Web 服务器
或 IIS 服务器等），通过接收客户端的请求并向客户端发送 HTTP 响应数据。
HTTP 使用统一资源标识符（Uniform Resource Identifiers, URI）来传输数据和建立连接。
一旦建立连接后，数据消息就通过类似 Internet 邮件所使用的格式[RFC5322]和多用途
Internet 邮件扩展（MIME）[RFC2045]来传送。
##客户端请求消息
和护短发送一个HTTP请求到服务器的请求消息包括以下格式：请求行（request line）、请求
头部（header）、空行和请求数据四个部分组成
##服务器响应消息 
HTTP 响应也由四个部分组成，分别是：状态行、消息报头、空行和响应正文。
##HTTP请求方法
根据 HTTP 标准，HTTP 请求可以使用多种请求方法。
HTTP1.0 定义了三种请求方法： GET, POST 和 HEAD 方法。
HTTP1.1 新增了六种请求方法：OPTIONS、PUT、PATCH、DELETE、TRACE 和 CONNECT 方法。
| 序号 |  类型  |  描述                                                 |
| ---  |  ----- | -----------------------------------------------------|
|   1  |  GET   | 请求指定的页面信息，并返回实体主体。                  |
|   2  |  HEAD  | 类似于 GET 请求，只不过返回的响应中没有具体的内容，</br>用于获取报头|
|   3  |  POST  | 向指定资源提交数据进行处理请求（例如提交表单或者上传文件）。数据被包含在请求体中。POST 请求可能会||      |        |导致新的资源的建立和/或已有资源的修改。                |