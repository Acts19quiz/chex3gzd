/*https://stackoverflow.com/questions/3842614/how-do-i-call-a-javascript-function-on-page-load*/
/*https://stackoverflow.com/questions/9456289/how-to-make-a-div-visible-and-invisible-with-javascript*/
var tooltiptext = document.getElementById('tools');
/*https://stackoverflow.com/questions/20377835/how-to-get-css-class-property-in-javascript*/
var elem = document.querySelector('.tooltiptext');
var tooltiptextstyle = getComputedStyle(elem);
/*https://www.w3schools.com/jsref/dom_obj_style.asp*/
var tooltipmaxWidth = tooltiptextstyle.maxWidth;
var tooltippaddingLeft = tooltiptextstyle.paddingLeft;
var tooltipborderLeftWidth = tooltiptextstyle.borderLeftWidth;
/*https://www.tutorialspoint.com/how-to-convert-a-pixel-value-to-a-number-value-using-javascript*/
var tooltipCSSwidth = parseInt(tooltipmaxWidth) + (parseInt(tooltippaddingLeft) * 2) + (parseInt(tooltipborderLeftWidth) * 2);

window.onload = function()
{
	setter();
};

/*https://stackoverflow.com/questions/15205609/call-a-function-when-window-is-resized*/
window.onresize = function()
{
	setter();
};

/*https://stackoverflow.com/questions/442404/retrieve-the-position-x-y-of-an-html-element*/
function getOffset(el)
{
	const rect = el.getBoundingClientRect();
	return{/*Doing this traditional block style (i.e., the brace on the next line) breaks everything because of `return`.*/
		left: rect.left + window.scrollX,
		right: rect.right + window.scrollX,
		bottom: rect.bottom + window.scrollY
	};
}

function setter()
{
	/*https://stackoverflow.com/questions/10118172/setting-div-width-and-height-in-javascript
	https://stackoverflow.com/questions/49044677/how-to-set-body-height-with-javascript*/
	var maindivrightedge = (getOffset(document.getElementById('maindiv')).right);
	var maindivleftedge = (getOffset(document.getElementById('maindiv')).left);
	var underlineleftedge = (getOffset(document.getElementById('underlined')).left);
	var mainwidth = maindivrightedge - maindivleftedge;
	var haus = maindivrightedge - underlineleftedge;
	var begs = tooltipCSSwidth - haus;
	var underlinebottomedge = (getOffset(document.getElementById('underlined')).bottom);
	tooltiptext.style.top = underlinebottomedge + 'px';
	/* The JavaScript recognized the entire width of the browser window in determining the x-position of an element (e.g., the tooltip). However, the browser's internal CSS/HTML goes by the relative position inside the main div.
	So we subtract everything to the left of the main div in order to get the JavaScript to set the x-position of our tooltip to something that makes sense for the browser.*/
	tooltiptext.style.left = (underlineleftedge - maindivleftedge) + 'px';
	/*If the distance between the right edges of the underlined tooltip and main div are less than the normal width of the tooltip text (tooltipCSSwidth, or 327px), we know the tooltip text has overflowed past its intended boundary.*/
	if (haus < tooltipCSSwidth)
	{
		tooltiptext.style.left = (underlineleftedge - maindivleftedge - begs) + 'px';
		if (mainwidth < tooltipCSSwidth)
		{
			tooltiptext.style.left = "0px";
			tooltiptext.style.right = "0px";
		}
	}
}

function hoverenter()
{
	tooltiptext.style.visibility = 'visible';
}

function hoverleave()
{
	tooltiptext.style.visibility = 'hidden';
}

/*https://www.reddit.com/r/css/comments/favj6m/keeping_tooltip_div_on_screen/
A click-based solution is needed for mobile devices.*/
function clicky()
{
	if (tooltiptext.style.visibility == 'hidden')
	{
		tooltiptext.style.visibility = 'visible';
	}
	else
	{
		tooltiptext.style.visibility = 'hidden';
	}
}