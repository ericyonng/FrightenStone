# Json库

![JSON for modern c++](https://raw.githubusercontent.com/nlohmann/json/master/doc/json.gif)(https://github.com/nlohmann/json/releases)

## 概述

> ​	本Json库移植至 nlohmann [json开源项目](<https://github.com/nlohmann/json> "Github:nlohmann/json") .该项目使用纯c++11编写，多人参与，并久经验证，具有不依赖第三方库，移植方便，使用方便，执行效率高等特点。

## 使用方法

> 若要创建如下的json：

```json
{
  "pi": 3.141,
  "happy": true,
  "name": "Niels",
  "nothing": null,
  "answer": {
    "everything": 42
  },
  "list": [1, 0, 2],
  "object": {
    "currency": "USD",
    "value": 42.99
  }
}
```

> 只需要:

```cpp
// create an empty structure (null)
json j;

// add a number that is stored as double (note the implicit conversion of j to an object)
j["pi"] = 3.141;

// add a Boolean that is stored as bool
j["happy"] = true;

// add a string that is stored as std::string
j["name"] = "Niels";

// add another null object by passing nullptr
j["nothing"] = nullptr;

// add an object inside the object
j["answer"]["everything"] = 42;

// add an array that is stored as std::vector (using an initializer list)
j["list"] = { 1, 0, 2 };

// add another object (using an initializer list of pairs)
j["object"] = { {"currency", "USD"}, {"value", 42.99} };

// instead, you could also write (which looks very similar to the JSON above)
json j2 = {
  {"pi", 3.141},
  {"happy", true},
  {"name", "Niels"},
  {"nothing", nullptr},
  {"answer", {
    {"everything", 42}
  }},
  {"list", {1, 0, 2}},
  {"object", {
    {"currency", "USD"},
    {"value", 42.99}
  }}
};
```

```cpp
// a way to express the empty array []
json empty_array_explicit = json::array();

// ways to express the empty object {}
json empty_object_implicit = json({});
json empty_object_explicit = json::object();

// a way to express an _array_ of key/value pairs [["currency", "USD"], ["value", 42.99]]
json array_not_object = json::array({ {"currency", "USD"}, {"value", 42.99} });
```

> 其他详细操作见作者的[github](<https://github.com/nlohmann/json> "Github:nlohmann/json")

## 感谢

> 再次感谢nlohmann提供的开源json库
>
> __作者__：[风焱辰](<https://github.com/ericyonng> "Github:ericyonng")
>
> __联系方式__: <120453674@qq.com>
>
> 