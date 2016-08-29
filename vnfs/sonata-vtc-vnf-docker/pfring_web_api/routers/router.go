package routers

import (
	"github.com/astaxie/beego"
	"pfring_web_api/controllers"
)

func init() {
	// Ajax
	beego.Router("/loadPFringModule", &controllers.PFringModuleController{}, "post:LoadPFringModule")
	beego.Router("/removePFringModule", &controllers.PFringModuleController{}, "post:RemovePFringModule")
	beego.Router("/getPFringModuleStatus", &controllers.PFringModuleController{}, "post:GetPFringModuleStatus")
	beego.Router("/getInterfaces", &controllers.InterfacesController{}, "post:GetInterfacesStatus")
	beego.Router("/setInterfacePromisc", &controllers.InterfacesController{}, "post:SetInterfacePromisc")
	beego.Router("/startPFbridge", &controllers.PFbridgeController{}, "post:StartPFbridge")
	beego.Router("/stopPFbridge", &controllers.PFbridgeController{}, "post:StopPFbridge")
	beego.Router("/checkPFbridge", &controllers.PFbridgeController{}, "post:CheckPFbridge")
}
