window.onload = function () {
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(mymap);
};

// inicia o mapa com o Rio de Janeiro centralizado
var mymap = L.map('mapid').setView([-22.912073, -43.2094], 13)

// inicia variáveis no escopo global
var cliques = 1
var url = window.location.href
var lat_origem
var lng_origem
var lat_destino
var lng_destino
var marker_origem
var marker_destino
var polyline

// desabilita as caixas de texto para o usuário
document.getElementById('latitude_origem').disabled = true
document.getElementById('longitude_origem').disabled = true
document.getElementById('latitude_destino').disabled = true
document.getElementById('longitude_destino').disabled = true
document.getElementById('tempo_execucao').disabled = true

function CalculaRota() {

    // coleta algoritmo selecionado
    var algoritmo = document.getElementById('algoritmo').value

    // envia os dados coletados ao servidor
    var myURL = "/api/" + lat_origem + "/" + lng_origem + "/" + lat_destino + "/" + lng_destino + "/" + algoritmo

    // faz request da resposta do servidor
    let myRequest = new Request(myURL)
    var nodes
    var time
    var dist

    fetch(myRequest)
        .then(function (resp) {

            // obtem o json retornado pelo servidor
            return resp.json()

        })
        .then(function (data) {

            // separa e armazena os dados do json em variáveis do escopo da função CalculaRota
            nodes = data.path
            time = data.time
            dist = data.dist
            time = Math.floor(time * 1000) / 1000
            dist = Math.floor(dist)

            console.log(data.visited)

            // mostra na interface o tempo de execução do algoritmo selecionado para tal rota
            document.getElementById('tempo_execucao').value = time + " segundos"

            _CalculaRota(nodes, dist)

        })
        .catch(function (err) {
            alert("O ponto clicado está fora do Rio de Janeiro")
            LimpaMarcadores();
        })
}

function _CalculaRota(nodes, dist) {

    if (polyline) {
        mymap.removeLayer(polyline)
    }

    // constroi um array com a sequência de nodes da rota

    var path = [[lat_origem, lng_origem]]
    var count = nodes.length

    for (i = 0; i < count; i++) {
        var aux = [nodes[i]["lat"], nodes[i]["long"]]
        path.push(aux)
    }

    path.push([lat_destino, lng_destino])

    // plota a rota no mapa
    polyline = L.polyline(path, { color: 'blue' }).addTo(mymap);
    polyline.bindPopup("<center><b>Rota mais curta:</b></center> Dist = " + dist + " metros").openPopup()

    // zoom na rota plotada
    mymap.fitBounds(polyline.getBounds());

}

function onMapClick(e) {
    if (cliques % 2 == 1) {

        if (cliques != 1) {
            mymap.removeLayer(marker_origem)
        }
        // coleta as coordenadas do ponto onde o usuário fez o primeiro clique
        lat_origem = e.latlng.lat
        lng_origem = e.latlng.lng

        // printa as coordenadas coletadas em sua respectiva caixa de texto
        document.getElementById('latitude_origem').value = lat_origem
        document.getElementById('longitude_origem').value = lng_origem

        // adiciona o marcador da origem com seu pop-up ao mapa
        marker_origem = L.marker([lat_origem, lng_origem]).addTo(mymap)
        marker_origem.bindPopup("<center><b>Origem:</b></center> Lat = " + lat_origem + ",<br> Lng = " + lng_origem).openPopup()

        cliques++

    } else if (cliques % 2 == 0) {


        if (cliques != 2) {
            mymap.removeLayer(marker_destino)
        }
        // coleta as coordenadas do ponto onde o usuário fez o segundo clique
        lat_destino = e.latlng.lat
        lng_destino = e.latlng.lng

        // printa as coordenadas coletadas em sua respectiva caixa de texto
        document.getElementById('latitude_destino').value = lat_destino
        document.getElementById('longitude_destino').value = lng_destino

        // adiciona o marcador do destino com seu pop-up ao mapa
        marker_destino = L.marker([lat_destino, lng_destino]).addTo(mymap)
        marker_destino.bindPopup("<center><b>Destino:</b></center> Lat = " + lat_destino + ",<br> Lng = " + lng_destino).openPopup()

        cliques++

    }
}

function LimpaMarcadores() {

    // remove os markers e a rota
    mymap.removeLayer(marker_origem)
    mymap.removeLayer(marker_destino)
    mymap.removeLayer(polyline)

}

// identifica cliques sobre o mapa e chama a função que vai tratá-los
mymap.on("click", onMapClick)
