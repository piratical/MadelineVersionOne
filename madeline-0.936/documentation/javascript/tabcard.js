//
// tabcard.js (c) 2004 Edward H. Trager
//

//
//  Set these variables according to the number of cards you have and the CSS properties
//  you have set for the tabs:
//
var cardCount=4;
var tabWidth=180;
var borderWidth=1;
var tabRightMargin=5;
var unit="px";
var tabDelta=tabWidth+2*tabRightMargin+3*borderWidth;
    
function displayCard(cardIndex){
	for(i=1;i<=cardCount;i++){
		id="card"+i;
		if(i==cardIndex){
			// Display the card:
			document.getElementById(id).style.display="block";
			// Place the "eraser" line in the proper location:
			j=borderWidth+(i-1)*tabDelta;
			document.getElementById("eraser").style.left=""+j+unit;
		}else{
			document.getElementById(id).style.display="none";
		}
	}
}
