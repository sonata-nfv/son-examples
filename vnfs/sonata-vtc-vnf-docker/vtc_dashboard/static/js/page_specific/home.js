//
// Global vars
//
    //var vtcHost      = '';
    //var dashbrdHost   = '';
    var btnStart     = true;




$(document).ready(function(){

  $("body").on("getPFringModuleStatusEvent", function(){
    getPFringModuleStatus(function(data){
      if (data.status == "fail") {
        $('.pfring-onoffswitch > input').prop( "checked", false );
        btn.prop('disabled',true)
      }
      else {
        $('.pfring-onoffswitch > input').prop( "checked", true );
        btn.prop('disabled',false)
      }
    });
  });


  $("body").on("getInterfacesEvent", function(){
    getInterfaces(function(data){
      // initialize select menus ("capture settings" section)
      initializeSelectMenus(data);

      // add markup after reading the incoming json with handlebars.js ("network interfaces" section)
      var source   = $("#ifaces-template").html();
      var template = Handlebars.compile(source);
      var html    = template(data);
      $(".ifaces-content").html(html);

      addPromiscSwitchListeners();

      checkPFbridge(function(data){
        if (data.status == "started") {
          btn.prop('disabled',false)
          toggleButton(btn);
          $('#in-selector').val(data.ifaceIN);
          $('#out-selector').val(data.ifaceOUT);
          lockInterfaceSelectors(true);
        }
        else {
          //
        }
      });
    });
  });

  // $.getJSON( "/static/json/grafana.json", function( data ) {
  //   dashbrdHost = data.url;
  //   $("body").trigger("getPFringModuleStatusEvent");
  //   $("body").trigger("getInterfacesEvent");
  // });

  $("body").trigger("getPFringModuleStatusEvent");
  $("body").trigger("getInterfacesEvent");




  btn       = $('.btn-wrapper button');
  pfringBtn = $('.pfring-onoffswitch > input');

  pfringBtn.on('change',function(){
    if ( pfringBtn.prop('checked') ) {
      loadPFringModule(function(data){
        if (data.status == "fail") {
          $('.pfring-onoffswitch > input').prop( "checked", false );
          btn.prop('disabled',true)
        }
        else {
          $('.pfring-onoffswitch > input').prop( "checked", true );
          btn.prop('disabled',false)
        }
      });
    }
    else {
      removePFringModule(function(data){
        if (data.status == "fail") {
          setTimeout(function(){
                $('.pfring-onoffswitch > input').prop( "checked", true );
                btn.prop('disabled',false)
          },1500);
        }
        else {
          $('.pfring-onoffswitch > input').prop( "checked", false );
          btn.prop('disabled',true)
        }
      });
    }
  })
  
  
  btn.click(function(){
    
    if ( btnStart ) {
      netSettings = buildNetConfig();
      if (netSettings == null) return;

      startPFbridge( netSettings, function(data){
        toggleButton(btn);
        lockInterfaceSelectors(true);
        setTimeout(updateInterfacePromisc,2000);
      });
    }
    else {
      stopPFbridge(function(data){
        //console.log(data);
        toggleButton(btn);
        lockInterfaceSelectors(false);
      });
    }
    
  });
});




function initializeSelectMenus(data) {
  for (var i=0 ; i<data.length ; i++) {
    // $('#in-selector').append('<option value="' + data[i].name +'">'+ data[i].name +'</option>');
    // $('#out-selector').append('<option value="'+ data[i].name +'">'+ data[i].name +'</option>');
    $('#in-selector')
         .append($("<option></option>")
         .attr("value",data[i].name)
         .text(data[i].name));
    $('#out-selector')
         .append($("<option></option>")
         .attr("value",data[i].name)
         .text(data[i].name));    
  }
}





// 
// receives a status
// 'success' means PF_ring module is loaded
// 'fail' means the opposite
//
function getPFringModuleStatus(callback) {
    $.ajax({
        type: 'POST',
        url:  '/getPFringModuleStatus',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          $('.pf-ring-no-info').removeClass('hidden');
          pfringBtn.prop( "checked", false ).prop('disabled',true);
          btn.prop('disabled',true);
        },
        timeout: 3000
    }); 
}



// 
// receives a status
// 'success' means PF_ring module was loaded successfully
// 'fail' means it could not be started
//
function loadPFringModule(callback) {
    $.ajax({
        type: 'POST',
        url:  '/loadPFringModule',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          $('.pf-ring-could-not-load').toggleClass('hidden');
        },
        timeout: 3000
    }); 
}




// 
// receives a status
// 'success' means PF_ring module was removed successfully
// 'fail' means it could not be removed
//
function removePFringModule(callback) {
    $.ajax({
        type: 'POST',
        url:  '/removePFringModule',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          $('.pf-ring-could-not-remove').toggleClass('hidden');
        },
        timeout: 3000
    }); 
}




// 
// receives information from the server about its interfaces (name, up/down, multicast, broadcast) 
//
function getInterfaces(callback) {
    $.ajax({
        type: 'POST',
        url:  '/getInterfacesStatus',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          $('.net-iface-no-info').toggleClass('hidden');
        },
        timeout: 3000
    }); 
}



function startPFbridge(netconf,callback) {
    $.ajax({
        type: 'POST',
        url:  '/startPFbridge',
        crossDomain: true,
        dataType: 'json',
        data:      {"jsonIn":JSON.stringify(netconf)},
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          $('.krnl-mod-state-wrapper').prepend('<div class="alert alert-danger alert-dismissable"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>Server could not start PFbridge.</div>');
        },
        timeout: 3000
    }); 
}



function checkPFbridge(callback) {
    $.ajax({
        type: 'POST',
        url:  '/checkPFbridge',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          //
        },
        timeout: 3000
    }); 
}



function stopPFbridge(callback) {
    $.ajax({
        type: 'POST',
        url:  '/stopPFbridge',
        crossDomain: true,
        dataType: 'json',
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          console.log(err);          
        },
        timeout: 3000
    }); 
}


function toggleButton(btn) {
  if ( btnStart ){
    btn.removeClass("btn-success").addClass("btn-danger");
    btn.text("STOP")
    btnStart = false;
  }
  else {
    btn.removeClass("btn-danger").addClass("btn-success");
    btn.text("START")
    btnStart = true;
  }
}




function buildNetConfig() {
  if ( $('#in-selector').val() == "" || $('#out-selector').val() =="" ) {
      $(".no-selected-iface").removeClass('hidden');
      setTimeout(function(){
        $(".no-selected-iface").addClass('hidden');
      },8000);
      return null;
  }
  var netIn  = $('#in-selector option:selected').text();
  var netOut = $('#out-selector option:selected').text();
  return {netIN: netIn, netOUT: netOut};
}



function addPromiscSwitchListeners() {
  promiscSwitces = $('.promisc-onoffswitch > input');

  promiscSwitces.on('change',function(){
    
    settings = new Object();
    settings.name = $(this).attr('data-iface');

    if ( $(this).prop('checked') ) {
      settings.active = true;
    }
    else {
      settings.active = false;
    }

    var pmSwitch = $(this);

    setInterfacePromisc(settings,function(data){
      if (data.status == "fail") {
        pmSwitch.prop( "checked", !pmSwitch.prop( "checked") );
      }
    });
  })
}


function setInterfacePromisc(settings, callback){
    $.ajax({
        type: 'POST',
        url:  '/setInterfacePromisc',
        crossDomain: true,
        dataType: 'json',
        data:      {"jsonIn":JSON.stringify(settings)},
        success: function(data)
        {        
            if( callback ) callback(data);
        },
        error: function(err)
        {
          console.log(err);
        },
        timeout: 3000
    }); 
}




function updateInterfacePromisc() {
  getInterfaces(function(data){
    for ( var i=0 ; i<data.length ; i++ ) {
      $('#'+data[i].name+'-onoffswitch').prop('checked', data[i].promisc);
    }
  });
}



function lockInterfaceSelectors(setting) {
  $('#in-selector, #out-selector').prop('disabled',setting);
}