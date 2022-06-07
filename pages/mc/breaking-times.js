resElem = document.getElementById('result')
hasteElem = document.getElementById('haste')
fatigueElem = document.getElementById('mf')
tierElem = document.getElementById('tier')
blockElem = document.getElementById('block')
effElem = document.getElementById('eff')
OPBEGIN = '<option>'
OPEND = '</option>'
req = new XMLHttpRequest()
hvs = {}
tierdmg=
{
    hand: 1, wood: 2, stone: 4, iron: 6,
    diamond: 8, netherite: 9,
    gold: 12
}
function calcHardness()
{
    var blname = blockElem.value, hardness = hvs[blname]
    if(hardness === -1)
    {
        resElem.innerText = 'Unbreakable'
        return
    }
    var y = hardness * 30, n = hardness * 100
    var haste = parseInt(hasteElem.value), fatigue = parseInt(fatigueElem.value)
    var efflvl = parseInt(effElem.value)
    var dmg = tierdmg[tierElem.value] + efflvl * efflvl + (efflvl > 0)
    dmg *= haste / 5 + 1;
    for(var i = 0; i < fatigue; i++) dmg *= 0.3
    y = Math.ceil(y / dmg) / 20
    n = Math.ceil(n / dmg) / 20
    y = y < 0.1 ? 'Instantly' : y + 's'
    n = n < 0.1 ? 'instantly' : n + 's'
    resElem.innerText = `${y} if obtainable, ${n} otherwise`
}
evtHandler = (e) =>
{
    if(e.keyCode === 13)
        calcHardness()
}
document.addEventListener("keyup", evtHandler)
req.onreadystatechange = function()
{
    if(this.readyState === 4 && this.status === 200)
    {
        var res = this.responseText
        var ens = res.split('\n')
        selstr = ''
        for(var en of ens)
        {
            var enls = en.split(' ')
            var name = enls[0], hardness = parseFloat(enls[1])
            hvs[name] = hardness
            selstr += OPBEGIN + name + OPEND
        }
        blockElem.innerHTML = selstr
    }
}
req.open('GET', 'hardnesses.txt', true)
req.send()
