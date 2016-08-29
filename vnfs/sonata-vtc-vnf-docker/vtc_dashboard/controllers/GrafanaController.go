package controllers

import (
	"fmt"
	"github.com/astaxie/beego"
	//"log"
	//"os"
	//"os/exec"
	"bytes"
	"net/http"
	//"strings"
	"encoding/json"
	"io/ioutil"
)

type GrafanaController struct {
	beego.Controller
}

type GrafanaJson struct {
	Url string `json:"url"`
}

type GrafanaApiKeyJson struct {
	Name string `json:"name"`
	Key  string `json:"key"`
}

var grafanaUrl string = ""
var grafanaApiKey string = ""

func readGrafanaUrl() {
	file, e := ioutil.ReadFile("./static/json/grafana.json")
	if e != nil {
		fmt.Printf("File error: %v\n", e)
		return
	}
	//fmt.Printf("%s\n", string(file))

	var fileJson GrafanaJson
	json.Unmarshal(file, &fileJson)
	grafanaUrl = fileJson.Url
}

func readGrafanaApiKey() {
	file, e := ioutil.ReadFile("./controllers/grafana_key.json")
	if e != nil {
		fmt.Printf("File error: %v\n", e)
		return
	}
	//fmt.Printf("%s\n", string(file))

	var fileJson GrafanaApiKeyJson
	json.Unmarshal(file, &fileJson)
	grafanaApiKey = fileJson.Key
}

func (c *GrafanaController) GetGrafanaDashboard() {
	readGrafanaUrl()
	if grafanaUrl == "" {
		fmt.Println("Grafana url is empty. (Check ./static/json/grafana.json)")
		return
	}
	url := grafanaUrl + "/api/dashboards/db/home"

	readGrafanaApiKey()
	if grafanaApiKey == "" {
		fmt.Println("Grafana API key is empty. (Check ./controllers/grafana_key.json)")
		return
	}

	var query = []byte(`your query`)
	req, err := http.NewRequest("GET", url, bytes.NewBuffer(query))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+grafanaApiKey)

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	// fmt.Println("response Status:", resp.Status)
	// fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	// fmt.Println("response Body:", string(body))

	c.Ctx.WriteString(string(body))
}

func (c *GrafanaController) UpdateGrafanaDashboard() {
	//
	// jsonIn has the following structure:
	// {netIN: "iface name", netOUT: "iface name"}
	//
	jsonIn := c.GetString("jsonIn")
	//fmt.Println("json in:", jsonIn)
	// data := []byte(jsonIn)
	// var ifcPromsc IfacePromisc
	// err := json.Unmarshal(data, &ifcPromsc)
	// checkErr(err)

	readGrafanaUrl()
	if grafanaUrl == "" {
		fmt.Println("Grafana url is empty. (Check ./static/json/grafana.json)")
		return
	}
	url := grafanaUrl + "/api/dashboards/db/"

	readGrafanaApiKey()
	if grafanaApiKey == "" {
		fmt.Println("Grafana API key is empty. (Check ./controllers/grafana_key.json)")
		return
	}

	var jsonStr = []byte(jsonIn)
	req, err := http.NewRequest("POST", url, bytes.NewBuffer(jsonStr))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Authorization", "Bearer "+grafanaApiKey)

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

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
