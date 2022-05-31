req = new XMLHttpRequest()
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
            var lnk = document.createElement('a')
            lnk.innerText = lnk.href = to
            lnk.className = 'dirent'
            var li = document.createElement('li')
            li.append(lnk)
            li.append(' ' + ftype)
            li.className = 'dirent'
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
