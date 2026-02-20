//
// iewarning.js (c) 2003 Edward H. Trager
//

//
// hackForIE():
//
function warnIEUsers(showWarning,navTopPosition){
	//alert(navigator.userAgent);
	//alert(navigator.userAgent.indexOf("MSIE"));
	//alert(navigator.userAgent.indexOf("Opera"));
	//alert(navigator.userAgent.indexOf("Gecko"));
	
	//alert(navigator.appName);
	//alert(navigator.appVersion);
	
	//
	// We can normally distinguish a true IE from, say, Opera masquerading as IE
	// from the presence of "MSIE" _and_ absence of "Opera" (or whatever other browser name)
	// in the userAgent string.  However some browsers, such as Konqueror, will give up all
	// traces of their own identity when implementing the "hack" to identify themselves as
	// IE.  However, since that is not the default behaviour, I feel the following is
	// sufficient for distinguishing within the class of recent DOM-enabled browsers:
	//
	//if(navigator.userAgent.indexOf("MSIE")){
	var agent=navigator.userAgent;
	if(agent.indexOf("MSIE")>0){
		//
		// Check against other browsers masquerading as IE:
		//
		if(agent.indexOf("Opera")>0) return;
		if(agent.indexOf("Gecko")>0) return;
		if(agent.indexOf("Konqueror")>0) return;
		if(agent.indexOf("KHTML")>0) return;
		if(agent.indexOf("Safari")>0) return;
		
		//
		// Warn the user about the problems with viewing in IE:
		//
		if(showWarning){
			var WarningParagraph = document.createElement("P");
			WarningParagraph.style.color="#a00";
			WarningParagraph.style.padding="3px";
			WarningParagraph.style.fontSize="14px";
			WarningParagraph.style.backgroundColor="#ff9";
			WarningParagraph.style.fontVariant="small-caps";
			var IEWarning = document.createTextNode("Warning: Internet Explorer has serious bugs in its implementation of CSS which cause this page to appear and behave differently than intended! Please use a recent version of Mozilla, Opera, Konqueror, Netscape, or Safari instead!");
			WarningParagraph.appendChild(IEWarning);
			document.getElementById("title").insertBefore(WarningParagraph,document.getElementById("title").firstChild);
		}
		if(navTopPosition){
			document.getElementById("title").style.top=navTopPosition;
		}
	}
	
}
