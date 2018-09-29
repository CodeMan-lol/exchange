<p align="center">
  <img width="200" height="200" src="./eos_dex_logo.png">
</p>

<div align="center">
	<h1>EOS DEX</h1>
	<h2>EOS Decentralized Exchanges</h2>
</div>

## 实现效果
目前可以实现EOS链上所有的EOS包括代币的交易买卖

## WIP
1. 3中不同的交易所在链上逻辑的盈利模式
2. 在考虑历史订单的处理方式，可能会有两个分支，如果资金足够可以将历史订单存到链上，在这个里面可能会提供删除历史订单等相应的操作。另一个分支不会提供历史订单这个功能。
3. 为了实现K线等常用功能，可能会再单开一个repo，通过热插件的方式来实现历史订单的存储
4. 持续修复bug

## BUG
. 买单的时候没有判断资金是否足够（待修复）
. 撤单权限的验证（待修复）

## develop 分支功能（未验证）
1. 添加了订单来源

## 功能

1. 下买单（bid）
2. 下卖单（ask）
3. 撤单  (cancelorder)

## 功能实现原理

> 买卖单的实现的核心原理是 低买高卖

1. 下买单的原理

    下买单主要分为以下这几步：
    . 首先会去查找链上的卖单，如果有匹配的就先把这部分订单的成交
    . 然后没有完成的买单会把剩下的资金存入到交易所这个账户中，并将这部分的挂买单写入到链上
    . 最后一步就将成交的订单买到的资金转入到买家的账户中
    
2. 下卖单的原理

    下卖单主要分为以下几个步骤：
    . 同样回去查找链上未完成的买单，如果有价格匹配的会先成交这部分的订单
    . 把没有完成的剩余资金存入到交易所中，并将这部分的卖单写入到链上
    . 将完成的订单的资金提现给卖家账户中
    
3. 撤单的实现

    撤单只能撤回没有完成的那部分订单，会在链上把这部分的数据删除，删除之前会将存入的资金退还到相应的账户中
    
## 部署
建议：可以在麒麟测试网上测试部署，贼好用

1. 将合约部署到链上。(unlock wallet)
```
cleos set contract <contract account> <contract_path(dir)>

cleos --url http://12x.x.x:xxxx --wallet-url http://xxx.xx.xx:xxx set contract hello11zhang /Users/zhangzhichao/off-work/eos/cmake-build-debug/contracts/exchange
```
2. 给合约代码授权转账操作（这一块是坑啊！！）
```
cleos --wallet-url http://127.0.0.1:6666 --url http://xx.xx.xx.xx:8888 set account permission hellozhang11 active '{"threshold": 1,"keys": [{"key": "EOS7AvcLQKShYhb2FHAybmjk9tqQAzJr9zaxyKjP9nwZ1RFr8ZoX8","weight": 1}],"accounts": [{"permission":{"actor":"hello11zhang","permission":"eosio.code"},"weight":1}]}' owner -p hellozhang11
```
> 这一部分的原理我会放在wiki中
    
## 用例
1. 下买单的示例
```
cleos --wallet-url http://127.0.0.1:6666 --url http://xx.xx.xx.xx:8888 push action hello11zhang bid '["hellozhang11","5.0000 ZZC",10,"0.0000 ZZC","onedexchange"]' --permission hellozhang11@active
```
abi:
```
{
      "name": "bid",
      "base": "",
      "fields": [{
          "name": "maker", //下单账户
          "type": "name"
        },{
          "name": "quantity", //下单要买的币种和数量
          "type": "asset"
        },{
          "name": "price", //价格  现在是用的uint64
          "type": "uint64"
        },{
          "name": "bid_currency", //要买币种的类型，但是因为symbol_type的问题，现在是asset类型，这样的话如上示例就需要写成 0.0000 XXX
          "type": "asset"
        },{
          "name": "bid_contract", //要买币种的合约   EOS的话eosio.token
          "type": "name"
}

```

2. 下卖单的示例
```
cleos --wallet-url http://127.0.0.1:6666 --url http://xx.xx.xx.xx:8888 push action hello11zhang ask '["onedexchange","1000.0000 ZZC",1,"onedexchange"]' --permission onedexchange@active
```
abi
```
{
      "name": "ask",
      "base": "",
      "fields": [{
          "name": "maker", //下单人
          "type": "name"
        },{
          "name": "quantity", //卖单的币种和类型
          "type": "asset"
        },{
          "name": "price", //价格
          "type": "uint64"
        },{
          "name": "ask_contract", //要卖币种的合约  eos是eosio.token
          "type": "name"
}
```

3. 撤单的示例
```
cleos --wallet-url http://127.0.0.1:6666 --url http://13.230.87.138:8888 push action hello11zhang cancelorder '["ask",1]' --permission onedexchange
```
abi
```
{
      "name": "cancelorder",
      "base": "",
      "fields": [{
          "name": "scope", //撤单种类
          "type": "name"
        },{
          "name": "order_id",  //撤单的id
          "type": "uint64"
        }
      ]
}
```

查询买卖单的示例(在这里面可以获取到订单号)
```
cleos --wallet-url http://127.0.0.1:6666 --url http://xx.xx.xx.xx:8888 get table hello11zhang ask orders
cleos --wallet-url http://127.0.0.1:6666 --url http://xx.xx.xx.xx:8888 get table hello11zhang bid orders
```

## 贡献方式 （之后会移到wiki中）
1. 帮忙翻译文档
2. 贡献代码
3. 提交bug修复

任何好的想法都欢迎，坐等来撩！

最近不知道怎么了，有的时候有通知，邮件会漏掉，如果回复不及时，加我微信也行（17611133971）  不过最好还是走github，这样可溯源，还可能对其他人有帮助。

## 感谢
