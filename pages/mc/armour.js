resElem = document.getElementById('result')
dmgElem = document.getElementById('dmg')
arElem = document.getElementById('armour')
toElem = document.getElementById('toughness')
protElem = document.getElementsByClassName('prot')
function calcDmg()
{
    var dmg = parseFloat(dmgElem.value), armour = parseInt(arElem.value), toughness = parseInt(toElem.value)
    var D = armour - dmg * 4 / (toughness + 8), minD = armour * 0.2
    D = Math.min(20, Math.max(minD, D)) * 0.04
    dmg -= dmg * D
    var epf = 0
    for(var el of protElem)
        epf += parseInt(el.value)
    epf = Math.min(20, epf) * 0.04
    dmg -= dmg * epf
    resElem.innerText = dmg
}
keyHandler = (e)=>
{
    if(e.keyCode === 13)
        calcDmg()
}
document.addEventListener("keyup", keyHandler)
