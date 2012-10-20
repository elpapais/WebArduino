var ipAdress = "192.168.1.177";

function xmlHttpObject()
{
	var xmlHttp = null;
	
	try {
		// Mozilla, Opera, Safari sowie Internet Explorer (ab v7)
		xmlHttp = new XMLHttpRequest();
	} catch(e) {
		try {
			// MS Internet Explorer (ab v6)
			xmlHttp  = new ActiveXObject("Microsoft.XMLHTTP");
		} catch(e) {
			try {
				// MS Internet Explorer (ab v5)
				xmlHttp  = new ActiveXObject("Msxml2.XMLHTTP");
			} catch(e) {
				xmlHttp  = null;
			}
		}
	}
	
	return xmlHttp;
}

function http(pin, state) {
	var xmlHttp = xmlHttpObject();
	var ipAdress = "192.168.1.177";
	
	//alert("digitalWrite("+pin+","+state+")");
	if (xmlHttp)
	{
		xmlHttp.open('GET', 'http://192.168.1.177/ardcmd:dw:'+pin+':'+state+';', true);
		//xmlHttp.open('GET', 'http://192.168.2.177/?param=digitalWrite(2,1)', true);
		xmlHttp.onreadystatechange = function ()
		{
			if (xmlHttp.readyState == 4) {
				document.getElementById("ergebnis").innerHTML = "Status von Pin " +pin+" auf " + ((state==1) ? "HIGH" : "LOW") + " setzen";
				//alert("Antwort:\n"+xmlHttp.responseText);
			}
		};
		
		xmlHttp.send(null);
	}
}

function analogRead()
{
	var xmlHttp = xmlHttpObject();
	var xmlResponse, data, root, parser, myResult;
	var ipAdress = "192.168.1.177";
	
	if (xmlHttp) {
		xmlHttp.open('GET', 'http://192.168.1.177/ardcmd:adc;', true);
		xmlHttp.onreadystatechange = function ()
		{
			if (xmlHttp.readyState == 4) {
				xmlResponse = xmlHttp.responseXML; //Text;
				//alert(""+xmlResponse);
				parser=new DOMParser();
				xmlDoc=parser.parseFromString(xmlResponse, "text/xml");
				data = xmlResponse.getElementsByName("adc_0")[0];
				//myResult = xmlDoc.getElementsByTagName("root")[0];//.getElementById("adc0"); //.firstChild.nodeValue;
				//data = myResult.getElementByName("adc0"); //("data")[0].nodeValue; //.getElementById("adc0");
				//xmlResponseTest = xmlResponse.getElementByTagName("data")[0];				//document.getElementById("ergebnis").innerHTML = "Status von Pin " +pin+" auf " + ((state==1) ? "HIGH" : "LOW") + " setzen";
				alert(""+data); //xmlHttp.responseText);
				
				//document.getElementById("adc0_int").innerHTML = "test: "+xmlResponseTest;
			}
		};
		xmlHttp.send(null);
	}
}

function allPins(state) {

	for(var i=2; i<=9; i++)
	{
		//alert("allPins "+i);
		http(i, state);
		
		if(state == 1)
		{
			document.getElementsByName("pin"+i)[0].checked = true;
			document.getElementsByName("pin"+i)[1].checked = false;
		}
		else
		{
			document.getElementsByName("pin"+i)[0].checked = false;
			document.getElementsByName("pin"+i)[1].checked = true;
		}	
	}
}

function switchButton(obj)
{
	var object = document.getElementById(obj);
	var i, n, pin;
	var splitStr = obj.split("_");
	
	if(splitStr[1] == "all")
	{
		n = 9;
		i = 2;
		
		if(document.getElementById(splitStr[0]+"_all").innerHTML == "OFF")
		{
			document.getElementById(splitStr[0]+"_all").innerHTML = "ON";
			document.getElementById(splitStr[0]+"_all").style.color = "green";
			//document.getElementById(splitStr[0]+"_all_button").color = "green";
		}
		else
		{
			document.getElementById(splitStr[0]+"_all").innerHTML = "OFF";
			document.getElementById(splitStr[0]+"_all").style.color = "red";
			//document.getElementById(splitStr[0]+"_all_button").color = "red";
		}
	}
	else
	{
		n = parseInt(splitStr[1]);
		i = n;	
	}	
	
	// alert(obj+":"+object.innerHTML);
	
	for(; i<=n; i++)
	{
		if(document.getElementById(splitStr[0]+"_"+i).innerHTML == "OFF")
		{
			document.getElementById(splitStr[0]+"_"+i).innerHTML = "ON";
			document.getElementById(splitStr[0]+"_"+i).style.color = "green";
			//document.getElementById(splitStr[0]+"_"+i+"_button").color = "green";
			
			http(i, 1);
		}
		else
		{
			document.getElementById(splitStr[0]+"_"+i).innerHTML = "OFF";
			document.getElementById(splitStr[0]+"_"+i).style.color = "red";
			//document.getElementById(splitStr[0]+"_"+i+"_button").color = "red";
			
			http(i, 0);
		}
	}
	
	return true;
}