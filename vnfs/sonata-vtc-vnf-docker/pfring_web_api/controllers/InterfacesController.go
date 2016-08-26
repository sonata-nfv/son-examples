package controllers

import (
	"encoding/json"
	"fmt"
	"github.com/astaxie/beego"
	"net"
	"os/exec"
	"strings"
)

type InterfacesController struct {
	beego.Controller
}

type Iface struct {
	Name      string `json:"name"`
	Status    string `json:"status"`
	Multicast bool   `json:"multicast"`
	Broadcast bool   `json:"broadcast"`
	Promisc   bool   `json:"promisc"`
}

func (c *InterfacesController) GetInterfacesStatus() {
	interfaces, err := net.Interfaces()
	if err != nil {
		fmt.Println(err)
		return
	}

	out, err := exec.Command("/bin/sh", "-c", "echo $USERPASS | sudo -S ifconfig").Output()
	if err != nil {
		fmt.Println(err)
		return
	}
	//fmt.Println(string(out))

	var ifaceset []Iface
	var ifc Iface
	var ifaceDetails = strings.Split(string(out), "\n\n")
	for _, i := range interfaces {

		//fmt.Printf("Name : %v \n", i.Name)
		// see http://golang.org/pkg/net/#Flags
		//fmt.Println("Interface type and supports : ", i.Flags.String())
		//fmt.Println(i.Flags.String())
		ifc.Name = i.Name
		if strings.Contains(i.Flags.String(), "up") {
			ifc.Status = "UP"
		} else {
			ifc.Status = "DOWN"
		}
		if strings.Contains(i.Flags.String(), "multicast") {
			ifc.Multicast = true
		} else {
			ifc.Multicast = false
		}
		if strings.Contains(i.Flags.String(), "broadcast") {
			ifc.Broadcast = true
		} else {
			ifc.Broadcast = false
		}
		for _, ifdetails := range ifaceDetails {
			if strings.Contains(ifdetails, i.Name) {
				if strings.Contains(ifdetails, "PROMISC") {
					fmt.Println(i.Name, " PROMISC UP")
					ifc.Promisc = true
				} else {
					ifc.Promisc = false
				}

			}
		}
		ifaceset = append(ifaceset, ifc)
	}

	ifacesJson, err := json.Marshal(ifaceset)
	//fmt.Println(string(ifacesJson))
	json := string(ifacesJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}

type IfacePromisc struct {
	Name   string `json:"name"`
	Active bool   `json:"active"`
}

func (c *InterfacesController) SetInterfacePromisc() {
	//
	// jsonIn has the following structure:
	// {netIN: "iface name", netOUT: "iface name"}
	//
	jsonIn := c.GetString("jsonIn")
	fmt.Println("json in:", jsonIn)
	data := []byte(jsonIn)
	var ifcPromsc IfacePromisc
	err := json.Unmarshal(data, &ifcPromsc)
	checkErr(err)
	//
	// the following commands switches on/off promisc mode
	//
	m := Message{"success"}

	if ifcPromsc.Active {
		_, err := exec.Command("/bin/sh", "-c", "echo $USERPASS | sudo -S ifconfig "+ifcPromsc.Name+" promisc").Output()

		if err != nil {
			fmt.Println("error:--------------------", err)
			m = Message{"fail"}
		}
	} else {
		_, err := exec.Command("/bin/sh", "-c", "echo $USERPASS | sudo -S ifconfig "+ifcPromsc.Name+" -promisc").Output()
		if err != nil {
			fmt.Println("error:--------------------", err)
			m = Message{"fail"}
		}
	}
	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}
