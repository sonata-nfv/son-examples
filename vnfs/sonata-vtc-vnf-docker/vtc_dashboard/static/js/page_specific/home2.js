//  eyJrIjoiYkdURk91VkNQSTF3OVdBWmczYUNoYThPOGs0QTJWZVkiLCJuIjoidGVzdDEiLCJpZCI6MX0=


grafanaHost    = '';
var dashbdJSON = null;
var graphsJSON = null;

var storedGraphJSON = new Array();

$(document).ready(function(){

  $("body").on("loadGrafanaIframe", function(){
    grafanaGetHome2Dashboard( function(data){
      dashbdJSON = data;
      graphsJSON = dashbdJSON.dashboard.rows[1].panels;

      $.each(graphsJSON,function( index, value ){
        //console.log(index+" " +value.title);
        $('.ichk-'+value.title).iCheck('check');
      });

      $('.icheck-wrapper input').on('ifChecked', function(event){
        dashboardAdd($(this).attr('data-name'));
      });

      $('.icheck-wrapper input').on('ifUnchecked', function(event){
        dashboardRemove($(this).attr('data-name'));
      });
    });
  });


  $('#grafana-iframe').load(function() { 
    setTimeout(function(){
      $('.black-layer').css('background-color','transparent');
      $('.black-layer > span').css('color','transparent');
    },500);
  });
  

  $.getJSON( "/static/json/grafana.json", function( data ) {
    grafanaHost = data.url;
    $('#grafana-iframe').attr('src',grafanaHost + '/dashboard/db/home');
    $("body").trigger("loadGrafanaIframe");
  });
   
  iCheckInit();
  loadGraphsJson();
});



function iCheckInit() {
  $('.icheck-wrapper input').each(function(){
    var self = $(this),
      label = self.next(),
      label_text = label.text();

    label.remove();
    self.iCheck({
      checkboxClass: 'icheckbox_line-custom',
      insert: '<div class="icheck_line-icon"></div>' + label_text
    });
  });
}



function grafanaGetHome2Dashboard(callback) {
  $.ajax({
      type: 'GET',
      url:  '/getGrafanaDashboard',
      //crossDomain: true,
      dataType: 'json',
      // headers: {
      //     "Authorization": "Bearer eyJrIjoiYkdURk91VkNQSTF3OVdBWmczYUNoYThPOGs0QTJWZVkiLCJuIjoidGVzdDEiLCJpZCI6MX0="
      // },
      success: function(data)
      {     
          if( callback ) callback(data);
      },
      error: function(err)
      {
        console.log(err);
      }
  }); 
}


function grafanaUpdateHome2Dashboard(callback) {
  var updtdJson = dashbdJSON;
  delete updtdJson.meta;
  updtdJson.overwrite = true;

  $.ajax({
      type: 'POST',
      url:  '/updateGrafanaDashboard',
      dataType: 'json',
      data: { "jsonIn" : JSON.stringify(updtdJson) },
      success: function(data)
      {     
          if( callback ) callback(data);
      },
      error: function(err)
      {
        console.log(err);
      }
  }); 
}


function dashboardAdd(graph) {
  graphsJSON.push(storedGraphJSON[graph]);
  dashbdJSON.dashboard.rows[1].panels = graphsJSON;
  grafanaUpdateHome2Dashboard(function(data){
    if (data.status == "success") {
      $('.black-layer').css('background-color','#161616');
      $('.black-layer > span').css('color','#777777');
      setTimeout(function(){
        $( '#grafana-iframe' ).attr( 'src', function ( i, val ) { return val; });
      },300);
    }
  });
}

function dashboardRemove(graph) {

  graphsJSON = $.grep(graphsJSON, function (value, index) {
    if ( value.title == graph ) {
      return false; // remove the element in the array
    }

    return true; // keep the element in the array
  });

  dashbdJSON.dashboard.rows[1].panels = graphsJSON;

  grafanaUpdateHome2Dashboard(function(data){
    if (data.status == "success") {
      $('.black-layer').css('background-color','#161616');
      $('.black-layer > span').css('color','#777777');
      setTimeout(function(){
        $( '#grafana-iframe' ).attr( 'src', function ( i, val ) { return val; });
      },300);
    }
  });
}


function loadGraphsJson() {
  $.getJSON( "/static/json/google.json", function( data ) {
    storedGraphJSON['Google'] = data;
  });

  $.getJSON( "/static/json/youtube.json", function( data ) {
    storedGraphJSON['Youtube'] = data;
  });

  $.getJSON( "/static/json/http.json", function( data ) {
    storedGraphJSON['HTTP'] = data;
  });

  $.getJSON( "/static/json/skype.json", function( data ) {
    storedGraphJSON['Skype'] = data;
  });

  $.getJSON( "/static/json/dropbox.json", function( data ) {
    storedGraphJSON['Dropbox'] = data;
  });

  $.getJSON( "/static/json/twitter.json", function( data ) {
    storedGraphJSON['Twitter'] = data;
  });

  $.getJSON( "/static/json/bittorrent.json", function( data ) {
    storedGraphJSON['Bittorrent'] = data;
  });
}