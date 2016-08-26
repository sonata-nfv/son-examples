package controllers

import (
	"fmt"
	"github.com/astaxie/beego"
	//"log"
	//"os"
	//"os/exec"
	"bytes"
	"net/http"
	"net/url"
	//"strings"
	"encoding/json"
	"io/ioutil"
)

type PFringController struct {
	beego.Controller
}

type PFringJson struct {
	Url string `json:"url"`
}

var pfringHostUrl string = ""

func readPFringHostUrl() {
	file, e := ioutil.ReadFile("./static/json/vtcHost.json")
	if e != nil {
		fmt.Printf("File error: %v\n", e)
		return
	}
	//fmt.Printf("%s\n", string(file))

	var fileJson PFringJson
	json.Unmarshal(file, &fileJson)
	pfringHostUrl = fileJson.Url
}


func sendPostRequest(c *PFringController, path string) {
	readPFringHostUrl()
	if pfringHostUrl == "" {
		fmt.Println("PFring-Host url is empty. (Check ./static/json/vtcHost.json)")
		return
	}
	url := pfringHostUrl + path


	var query = []byte(`your query`)
	req, err := http.NewRequest("POST", url, bytes.NewBuffer(query))
	req.Header.Set("Content-Type", "application/json")


	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	// fmt.Println("response Status:", resp.Status)
	// fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))

	c.Ctx.WriteString(string(body))
}


func (c *PFringController) GetPFringModuleStatus() {
	sendPostRequest(c, "/getPFringModuleStatus/")
}

func (c *PFringController) GetInterfacesStatus() {
	sendPostRequest(c, "/getInterfaces/")
}

func (c *PFringController) CheckPFbridge() {
	sendPostRequest(c, "/checkPFbridge/")
}

func (c *PFringController) LoadPFringModule() {
	sendPostRequest(c, "/loadPFringModule/")
}

func (c *PFringController) RemovePFringModule() {
	sendPostRequest(c, "/removePFringModule/")
}

func (c *PFringController) StopPFbridge() {
	sendPostRequest(c, "/stopPFbridge/")
}


func (c *PFringController) StartPFbridge() {
	jsonIn := c.GetString("jsonIn")

	readPFringHostUrl()
	if pfringHostUrl == "" {
		fmt.Println("PFring-Host url is empty. (Check ./static/json/vtcHost.json)")
		return
	}
	urlStr := pfringHostUrl + "/startPFbridge/"

	data := url.Values{}
    data.Set("jsonIn", jsonIn)
	req, _ := http.NewRequest("POST", urlStr, bytes.NewBufferString(data.Encode())) // <-- URL-encoded payload
    req.Header.Add("Content-Type", "application/x-www-form-urlencoded")

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	// fmt.Println("response Status:", resp.Status)
	// fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))

	c.Ctx.WriteString(string(body))
}


func (c *PFringController) SetInterfacePromisc() {
		jsonIn := c.GetString("jsonIn")

		readPFringHostUrl()
		if pfringHostUrl == "" {
			fmt.Println("PFring-Host url is empty. (Check ./static/json/vtcHost.json)")
			return
		}
		urlStr := pfringHostUrl + "/setInterfacePromisc/"

		data := url.Values{}
	    data.Set("jsonIn", jsonIn)
		req, _ := http.NewRequest("POST", urlStr, bytes.NewBufferString(data.Encode())) // <-- URL-encoded payload
	    req.Header.Add("Content-Type", "application/x-www-form-urlencoded")

		client := &http.Client{}
		resp, err := client.Do(req)
		if err != nil {
			panic(err)
		}
		defer resp.Body.Close()

		// fmt.Println("response Status:", resp.Status)
		// fmt.Println("response Headers:", resp.Header)
		body, _ := ioutil.ReadAll(resp.Body)
		fmt.Println("response Body:", string(body))

		c.Ctx.WriteString(string(body))
}
