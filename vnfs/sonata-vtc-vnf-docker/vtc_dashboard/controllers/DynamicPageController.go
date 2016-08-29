package controllers

import (
	"github.com/astaxie/beego"
)

type Home2Controller struct {
	beego.Controller
}

func (c *Home2Controller) Get() {
	// c.Data["Website"] = "beego.me"
	// c.Data["Email"] = "astaxie@gmail.com"
	c.TplName = "home2.html"
}
