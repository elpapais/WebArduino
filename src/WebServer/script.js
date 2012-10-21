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
	
	//alert("digitalWrite("+pin+","+state+")");
	if (xmlHttp)
	{
		xmlHttp.open('GET', 'http://'+ipAdress+'/ardcmd:dw:'+pin+':'+state+';', true);
		xmlHttp.onreadystatechange = function ()
		{
			if (xmlHttp.readyState == 4) {
				document.getElementById("ergebnis").innerHTML = "Status von Pin " +pin+" auf " + ((state==1) ? "HIGH" : "LOW") + " setzen";
				// alert("Antwort:\n"+xmlHttp.responseText);
			}
		};
		
		xmlHttp.send(null);
	}
}

function checkPinState()
{
				// dout_2 bis dout_9 auslesen
				for(var i=0; i<=7; i++)
				{
					var node = xmlDoc.getElementsByTagName("data")[i];
					var value = parseInt(node.firstChild.data); // ("dout_2");
					var adc_port = i-8;
					
					//alert ("value is: "+ value);
					
					document.getElementById("dout"+adc_port+"_int").innerHTML = value;
					document.getElementById("adc"+adc_port+"_volt").innerHTML = Math.round(parseFloat(value)*vFactor * 100) / 100;
				}
	}

function analogRead()
{
	var xmlHttp = xmlHttpObject();
	var data, root, parser, myResult;
	var vFactor = 5.0 / 1023.0;
	
	if (xmlHttp) {
		xmlHttp.open('GET', 'http://'+ipAdress+'/ardcmd:adc;', true);
		xmlHttp.onreadystatechange = function ()
		{
			if (xmlHttp.readyState == 4) {
				var xmlResponse = xmlHttp.responseText;
				//alert("analogRead:\n"+xmlResponse);
				
				xmlDoc = domParser(xmlResponse);
				//var customerNode = xmlDoc.getElementById('dout_2');
				//alert("test: "+customerNode);
								
				// ADC auslesen
				for(var i=8; i<=13; i++)
				{
					var node = xmlDoc.getElementsByTagName("data")[i];
					var value = node.firstChild.data; // ("dout_2");
					var adc_port = i-8;
					
					//alert ("value is: "+ value);
					
					document.getElementById("adc"+adc_port+"_int").innerHTML = value;
					document.getElementById("adc"+adc_port+"_volt").innerHTML = Math.round(parseFloat(value)*vFactor * 100) / 100;
				}
				
				//document.getElementById("adc0_int").innerHTML = "test: "+xmlResponseTest;
			}
		};
		xmlHttp.send(null);
	}
	
	return true;
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
	var prefix = splitStr[0];
	var sufix = splitStr[1];
	
	if(sufix == "all")
	{
		n = 9;
		i = 2;
		
		switchElementTextById(prefix, sufix); // Farbanpassung von switch-all button
	}
	else
	{
		n = parseInt(splitStr[1]);
		i = n;	
	}	
	
	for(; i<=n; i++)
	{
		switchElementTextById(prefix, i);
		
		if(document.getElementById(prefix+"_"+i).innerHTML == "ON")
			http(i, 1);
		else
			http(i, 0);
	}
	
	return true;
}

function switchElementTextById(prefix, sufix)
{
	if(document.getElementById(prefix+"_"+sufix).innerHTML == "OFF")
	{
		document.getElementById(prefix+"_"+sufix).innerHTML = "ON";
		document.getElementById(prefix+"_"+sufix).style.color = "green";
	}
	else
	{
		document.getElementById(prefix+"_"+sufix).innerHTML = "OFF";
		document.getElementById(prefix+"_"+sufix).style.color = "red";
	}

}

function domParser(text)
{
	var xmlDoc;
	
	if (window.DOMParser)
	{
	  parser=new DOMParser();
	  xmlDoc=parser.parseFromString(text, "text/xml");
	}
	else // Internet Explorer
	{
	  xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
	  xmlDoc.async=false;
	  xmlDoc.loadXML(text);
	}
	
	return xmlDoc;
}