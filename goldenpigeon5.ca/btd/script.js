req = new XMLHttpRequest()
req.onreadystatechange = function()
{
    if(this.readyState == 4 && this.status == 200)
    {
        direntstr = this.responseText
        ens = direntstr.split('\n')
        lsdir = document.getElementById('ls')
        ens.pop()
        for(var to of ens)
        {
            var lnk = document.createElement('a')
            lnk.innerText = lnk.href = to
            lnk.className = 'dirent'
            var li = document.createElement('li')
            li.append(lnk)
            lsdir.append(li)
        }
    }
}
req.open('GET', 'dirent.txt', true)
req.send()
