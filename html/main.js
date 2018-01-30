function cupsOnClick() {
    httpGet("/cups");
    var site = document.getElementById("cups");
    cupsCount++;
    if (cupsCount > cupsImages.length) {
      cupsCount = 0;
    }
    site.src = cupsImages[cupsCount];
}

function strengthOnClick() {
    httpGet("/strength");
    var e = document.getElementById("strength");
    strengthCount++, strengthCount == strengthImages.length && (strengthCount = 0), e.src = strengthImages[strengthCount]
}

function grinderOnClick() {
    httpGet("/grinder");
    var e = document.getElementById("grinder");
    grinderCount++, grinderCount == grinderImages.length && (grinderCount = 0), e.src = grinderImages[grinderCount]
}

function startOnClick() {
    start = 1;
    var e = document.getElementsByClassName("button");
    for (i = 0; i < e.length; i++) e[i].disabled = !0;
    var t = document.getElementsByClassName("buttonImage");
    for (i = 0; i < t.length; i++) t[i].classList.add("imagetint");
    switch (amountOfCups[cupsCount]) {
        case 2:
            setTimeout(showNotification, 1e4);
            break;
        case 4:
            setTimeout(showNotification, 1e4);
            break;
        case 6:
            setTimeout(showNotification, 1e4);
            break;
        case 8:
            setTimeout(showNotification, 1e4);
            break;
        case 10:
            setTimeout(showNotification, 1e4)
    }
    httpGet("/start")
}

function setCoffeeText(e, t) {
    if (console.log(t), "ok" !== t) {
        var n = document.getElementById("coffeeText");
        n.innerHTML = "\n\n There was an error processing your request please try again"
    }
    if ("ok" === t && "/start" === e) {
        var n = document.getElementById("coffeeText");
        n.innerHTML = "\n\n Your " + amountOfCups[cupsCount] + " cups of coffee are getting ready, please wait for the notification"
    }
}

function clearCoffeeText() {
    var e = document.getElementById("coffeeText");
    e.innerHTML = ""
}

function showNotification() {
    start = 0, clearCoffeeText();
    var e = document.getElementsByClassName("button");
    for (i = 0; i < e.length; i++) e[i].disabled = !1;
    var t = document.getElementsByClassName("buttonImage");
    for (i = 0; i < t.length; i++) t[i].classList.remove("imagetint");
    window.navigator.vibrate(500);
    var n = {
            icon: "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/cups.png",
            body: "Your coffee is ready!"
        },
        o = new Notification("CoffeePot", n);
    o.onclick = function() {
        window.open("http://coffeepot.local")
    }
}

function httpGet(e) {
    var t = new XMLHttpRequest;
    t.onreadystatechange = function() {
        t.readyState == XMLHttpRequest.DONE && setCoffeeText(e, t.responseText)
    }, t.open("GET", e, !0), t.send(null)
}
var twoCups = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/twocups.png",
    fourCups = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/fourcups.png".
    sixCups = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/sixcups.png",
    eightCups = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/eightcups.png",
    tenCups = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/tencups.png",
    mild = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/mild.png",
    medium = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/medium.png"
    strong = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/strong.png",
    grinderOn = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/grinderon.png",
    grinderOff = "https://raw.githubusercontent.com/wilmardo/CoffeePot/master/images/grinderoff.png",
    cupsImages = [twoCups, fourCups, sixCups, eightCups, tenCups],
    strengthImages = [mild, medium, strong],
    grinderImages = [grinderOn, grinderOff],
    amountOfCups = [2, 4, 6, 8, 10];
document.addEventListener("DOMContentLoaded", function() {
    "denied" !== Notification.permission && Notification.requestPermission(), document.getElementById("cups").src = cupsImages[cupsCount], document.getElementById("strength").src = strengthImages[strengthCount], document.getElementById("grinder").src = grinderImages[grinderCount]
});
