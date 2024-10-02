var urltext = document.getElementById('urlhere');
/*https://stackoverflow.com/questions/3490658/display-current-url-of-webpage-dynamic-in-html*/
urltext.innerHTML = window.location.href;
/*https://www.w3schools.com/jsref/prop_style_textdecoration.asp*/
urltext.style.textDecoration = "underline";
/*https://www.w3schools.com/jsref/prop_style_textdecorationstyle.asp*/
urltext.style.textDecorationStyle = "dotted";