# 解码内网服务端rtsp流， 并推送部分图片到http服务器

# 简单的cas，自旋
* 由于是cas循环自旋,cpu消耗较大，故采用sleep
* 实时要求不高、可通过condition_variable, 事件(如io管道)等优化

# bug修复
* [compare_exchange_strong](http://www.cplusplus.com/reference/atomic/atomic/compare_exchange_strong/)
* if true, it replaces the contained value with val (like store).
* if false, it replaces expected with the contained value
* 在false情况下，会对expected值做处理，而不是什么都没做


