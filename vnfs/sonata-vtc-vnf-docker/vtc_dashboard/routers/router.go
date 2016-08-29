package routers

import (
	"github.com/astaxie/beego"
	"vtc_dashboard/controllers"
)

func init() {
	beego.Router("/", &controllers.Home2Controller{})      // home page
	beego.Router("/home", &controllers.HomeController{})   // home page
	beego.Router("/home2", &controllers.Home2Controller{}) // dynamic page
	beego.Router("/getGrafanaDashboard", &controllers.GrafanaController{}, "get:GetGrafanaDashboard")
	beego.Router("/updateGrafanaDashboard", &controllers.GrafanaController{}, "post:UpdateGrafanaDashboard")
	
	// routing to PFring Host
	beego.Router("/loadPFringModule", &controllers.PFringController{}, "post:LoadPFringModule")
	beego.Router("/removePFringModule", &controllers.PFringController{}, "post:RemovePFringModule")
	beego.Router("/getPFringModuleStatus", &controllers.PFringController{}, "post:GetPFringModuleStatus")
	beego.Router("/getInterfacesStatus", &controllers.PFringController{}, "post:GetInterfacesStatus")
	beego.Router("/setInterfacePromisc", &controllers.PFringController{}, "post:SetInterfacePromisc")
	beego.Router("/startPFbridge", &controllers.PFringController{}, "post:StartPFbridge")
	beego.Router("/stopPFbridge", &controllers.PFringController{}, "post:StopPFbridge")
	beego.Router("/checkPFbridge", &controllers.PFringController{}, "post:CheckPFbridge")
}