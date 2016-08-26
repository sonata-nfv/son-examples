package controllers

import (
	//"bytes"
	"encoding/json"
	"fmt"
	"github.com/astaxie/beego"
	"log"
	//"os"
	"os/exec"
	//"strings"
	//"time"
	"bufio"
	"io/ioutil"
	"os"
	//"path/filepath"
	"strconv"
)

type PFbridgeController struct {
	beego.Controller
}

type NetConfig struct {
	NetIN  string `json:"netIN"`
	NetOUT string `json:"netOUT"`
}

type MessagePFbridgeStatus struct {
	Status   string `json:"status"`
	IfaceIN  string `json:"ifaceIN"`
	IfaceOUT string `json:"ifaceOUT"`
}

type InfluxDBJson struct {
	Url string `json:"url"`
}

var proID int = -1
var inIface string = ""
var outIface string = ""
var influxDBurl string = ""

func readInfluxDBurl() {
	file, e := ioutil.ReadFile("./controllers/influxDB.json")
	if e != nil {
		fmt.Printf("File error: %v\n", e)
		return
	}
	fmt.Printf("%s\n", string(file))

	var fileJson InfluxDBJson
	json.Unmarshal(file, &fileJson)
	influxDBurl = fileJson.Url
}

func (c *PFbridgeController) StartPFbridge() {
	readInfluxDBurl()
	//
	// jsonIn has the following structure:
	// {netIN: "iface name", netOUT: "iface name"}
	//
	jsonIn := c.GetString("jsonIn")
	fmt.Println("json in:", jsonIn)
	data := []byte(jsonIn)
	var netConfig NetConfig
	err := json.Unmarshal(data, &netConfig)
	checkErr(err)
	//
	// the following command starts pfbridge
	//
	cmd := exec.Command("/bin/sh", "-c", "sudo /root/gowork/src/pfring_web_api/vtc/PF_RING/userland/examples/pfbridge -a "+netConfig.NetIN+" -b "+netConfig.NetOUT+" -d "+influxDBurl)
	cmdReader, err := cmd.StdoutPipe()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error creating StdoutPipe for Cmd", err)
		os.Exit(1)
	}

	scanner := bufio.NewScanner(cmdReader)
	go func() {
		for scanner.Scan() {
			fmt.Printf(string(scanner.Text()))
		}
	}()

	m := Message{"started"}
	err = cmd.Start()
	proID = cmd.Process.Pid
	inIface = netConfig.NetIN
	outIface = netConfig.NetOUT
	fmt.Println("PID: ", proID)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error starting Cmd", err)
		//os.Exit(1)
	}

	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)

	// err = cmd.Wait()
	// if err != nil {
	// 	fmt.Fprintln(os.Stderr, "Error waiting for Cmd", err)
	// 	os.Exit(1)
	// }
}

func (c *PFbridgeController) CheckPFbridge() {
	m := MessagePFbridgeStatus{"started", inIface, outIface}
	//m := Message{"started"}
	if proID == -1 {
		m = MessagePFbridgeStatus{"not started", "", ""}
	}

	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}

func (c *PFbridgeController) StopPFbridge() {
	m := Message{"stopped"}

	// pro, err := os.FindProcess(proID)
	// if err != nil {
	// 	fmt.Fprintln(os.Stderr, "Error while finding process with pid: ", pro.Pid, "\nerror:", err)
	// 	m = Message{"Error while finding process"}
	// }

	// err = pro.Kill()
	// if err != nil {
	// 	fmt.Fprintln(os.Stderr, "Error while trying to kill process with pid: ", pro.Pid, "\nerror:", err)
	// 	m = Message{"Error while trying to kill process"}
	// }

	//args := "-c " + "echo 'ii70mseq' | sudo -S kill -- " + strconv.Itoa(proID)
	// fmt.Println(args)
	// cmd := exec.Command("/bin/sh", args)
	// err := cmd.Run()
	// if err != nil {
	// 	fmt.Fprintln(os.Stderr, "Error starting Cmd", err)
	// 	//os.Exit(1)
	// }

	//out, err := exec.Command("/bin/sh", "-c ", "echo 'ii70mseq' | sudo -S kill -- ", strconv.Itoa(proID)).Output()
	//out, err := exec.Command("/bin/sh", "-c ", "echo 'ii70mseq' | sudo -S pkill -TERM -P "+strconv.Itoa(proID)).Output()

	fmt.Println("/bin/sh", "-c", "sudo pkill -TERM -P "+strconv.Itoa(proID))
	cmd := exec.Command("/bin/sh", "-c", "sudo pkill -TERM -P "+strconv.Itoa(proID))
	cmdReader, err := cmd.StdoutPipe()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error creating StdoutPipe for Cmd", err)
		os.Exit(1)
	}

	scanner := bufio.NewScanner(cmdReader)
	go func() {
		for scanner.Scan() {
			fmt.Printf(string(scanner.Text()))
		}
	}()

	err = cmd.Start()

	if err != nil {
		fmt.Fprintln(os.Stderr, "Error starting Cmd: ", err)
		os.Exit(1)
	}

	proID = -1
	if err != nil {
		log.Fatal(err)
	}

	responseJson, err := json.Marshal(m)
	json := string(responseJson)
	checkErr(err)

	c.Ctx.WriteString(json)
}
