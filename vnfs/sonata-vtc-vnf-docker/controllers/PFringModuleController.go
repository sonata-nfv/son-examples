package controllers

import (
	"encoding/json"
	"fmt"
	"github.com/astaxie/beego"
	//"log"
	//"os"
	"os/exec"
	"strings"
)

type PFringModuleController struct {
	beego.Controller
}

type Message struct {
	Status string `json:"status"`
}

func (c *PFringModuleController) GetPFringModuleStatus() {
	out, err := exec.Command("/bin/sh", "-c", "sudo lsmod").Output()
	if err != nil {
		fmt.Println(err)
	}
	//fmt.Printf(string(out))

	m := Message{"fail"}

	if strings.Contains(string(out), "pf_ring") {
		m = Message{"success"}
	}
	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}

func (c *PFringModuleController) LoadPFringModule() {

	out, err := exec.Command("/bin/sh", "-c", "sudo insmod /root/gowork/src/pfring_web_api/vtc/PF_RING/kernel/pf_ring.ko min_num_slots=16384 enable_debug=1 quick_mode=1 enable_tx_capture=0").Output()
	fmt.Println(out)
	m := Message{"success"}
	if err != nil {
		fmt.Println(err)
		m = Message{"fail"}
	}

	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}

func (c *PFringModuleController) RemovePFringModule() {

	out, err := exec.Command("/bin/sh", "-c", "sudo rmmod /root/gowork/src/pfring_web_api/vtc/PF_RING/kernel/pf_ring.ko").Output()
	fmt.Println(out)
	m := Message{"success"}
	if err != nil {
		fmt.Println("error------------------------", err)
		m = Message{"fail"}
	}

	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
