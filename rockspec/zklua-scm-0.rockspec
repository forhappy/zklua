package = "zklua"
version = "scm-0"
source = {
   url = "git://github.com/forhappy/zklua.git",
   branch = "master"
}
description = {
   summary = "Lua binding for apache zookeeper.",
   homepage = "http://github.com/forhappy/zklua",
   license = "APACHE LICENSE/2.0",
   maintainer = "forhappy<haipingf@gmail.com>"
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "builtin",
   modules = {
      zklua = {
         sources = {"zklua.c"},
         libraries = {"zklua"}
      }
   }
}
