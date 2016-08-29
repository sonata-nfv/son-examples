package main

import (
	"github.com/astaxie/beego"
	_ "vtc_dashboard/routers"
)

func main() {
	//beego.TemplateLeft = "<<<"
	//beego.TemplateRight = ">>>"
	//beego.HttpPort = 8090
	beego.Run()
}
