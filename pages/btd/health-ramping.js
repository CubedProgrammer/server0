res = document.getElementById('result')
roundInput = document.getElementById('round')
chart = document.getElementById('chart')
basehp = [200, 700, 4000, 400, 20000]
document.addEventListener('keypress', onKP)
for(i = 81; i < 601; ++i)
{
    var scale = calcRamp(i)
    chr = document.createElement('tr')
    txtn = document.createTextNode(i)
    td = document.createElement('td')
    td.appendChild(txtn)
    chr.appendChild(td)
    txt = scale.toString(10) + '%'
    txtn = document.createTextNode(txt)
    td = document.createElement('td')
    td.appendChild(txtn)
    chr.appendChild(td)
    scale *= 0.01
    for(var h of basehp)
    {
        txt = Math.round(scale * h).toString(10)
        txtn = document.createTextNode(txt)
        td = document.createElement('td')
        td.appendChild(txtn)
        chr.appendChild(td)
    }
    chart.appendChild(chr)
}
function onKP(evt)
{
    if(evt.keyCode === 13)
        displayRamp()
}
function displayRamp()
{
    r = roundInput.value
    factor = calcRamp(r) * 0.01
    resstr = factor
    for(var h of basehp)
        resstr += ',' + Math.round(factor * h)
    res.innerText = resstr
}
function calcRamp(n)
{
    if(n < 81)
        return 100
    if(n < 101)
        return n * 2 - 60
    if(n < 125)
        return n * 5 - 360
    if(n < 151)
        return n * 15 - 1600
    if(n < 251)
        return n * 35 - 4600
    if(n < 301)
        return n * 100 - 20850
    if(n < 401)
        return n * 150 - 35850
    if(n < 501)
        return n * 250 - 75850
    else
        return n * 500 - 200850
}
