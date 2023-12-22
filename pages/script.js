currDate = new Date()
isChristmas = currDate.getMonth() == 11 && currDate.getDate() >= 24 && currDate.getDate() <= 26
req = new XMLHttpRequest()
if(isChristmas)
{
    var mc = document.createElement('h1')
    mc.innerText = 'MERRY CHRISTMAS'
    mc.style.color = 'red'
    document.body.insertBefore(mc, document.body.children[0])
    document.body.style.backgroundColor = 'green'
}
elemForFile = function(lnktxt, ftype)
{
    var lnk = document.createElement('a')
    lnk.innerText = lnk.href = lnktxt
    var li = document.createElement('div')
    var txtnode = document.createTextNode(' ' + ftype)
    li.append(lnk)
    li.append(txtnode)
    li.className = isChristmas ? 'christmasdirent' : 'dirent'
    return li
}
req.onreadystatechange = function()
{
    if(this.readyState == 4 && this.status == 200)
    {
        direntstr = this.responseText
        ens = direntstr.split('\n')
        lsdir = document.getElementById('ls')
        ens.pop()
        for(var en of ens)
        {
            var enls = en.split(' ')
            var to = enls[0], ftype = enls[1]
            var li = elemForFile(to, ftype)
            lsdir.append(li)
        }
    }
}
req.open('GET', 'dirent.txt', true)
req.send()
canv = document.getElementById('canv')
ctx = canv.getContext('2d')
canvasBG = 
{
    red: 255,
    green: 0,
    blue: 0
}
rainbowFunc = function()
{
    var num = canvasBG.red * 65536 + canvasBG.green * 256 + canvasBG.blue + 16777216
    var colstr = num.toString(16).substring(1)
    ctx.fillStyle = '#' + colstr
    ctx.fillRect(0, 0, canv.width, canv.height)
    if(canvasBG.red == 255)
    {
        if(canvasBG.green == 255)
            --canvasBG.red
        else if(canvasBG.blue > 0)
            --canvasBG.blue
        else
            ++canvasBG.green
    }
    else if(canvasBG.green == 255)
    {
        if(canvasBG.blue == 255)
            --canvasBG.green
        else if(canvasBG.red > 0)
            --canvasBG.red
        else
            ++canvasBG.blue
    }
    else if(canvasBG.blue == 255)
    {
        if(canvasBG.green > 0)
            --canvasBG.green
        else
            ++canvasBG.red
    }
}
rainbow = setInterval(rainbowFunc, 17)
