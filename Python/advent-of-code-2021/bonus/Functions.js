const apriFin = () => {
  const random = Math.floor(Math.random() * 500 + 300);
  const finestra = confirm("vuoi aprire una finestra?");
  if (finestra) {
    window.open("NuovaFin.html", "Nuova finestra", "height=" + random + "," + "width=" + random);
  }
};

const isValidHttpUrl = (string) => {
  const reg = /^(https?|ftp):\/\/([a-zA-Z0-9.-]+(:[a-zA-Z0-9.&%$-]+)*@)*((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]?)(\.(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])){3}|([a-zA-Z0-9-]+\.)*[a-zA-Z0-9-]+\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(:[0-9]+)*(\/($|[a-zA-Z0-9.,?'\\+&%$#=~_-]+))*$/;
  return reg.test(string);
};

const isValidPhoneNum = (string) => {
  return string.match(/^[0-9]+$/) != null;
};

const validateForm = () => {
  const nome = $("#nome").val();
  const tel = $("#tel").val();
  const sito = $("#sito").val();

  if (!nome) {
    alert("Il campo nome azienda è obbligatorio");
    return false;
  }

  if (!sito) {
    alert("Il campo sito web è obbligatorio");
    return false;
  }

  if (!tel) {
    alert("Il campo telefono è obbligatorio");
    return false;
  }

  if (!isValidHttpUrl(sito)) {
    alert("L'indirizzo web non è corretto");
    return false;
  }

  if (!isValidPhoneNum(tel)) {
    alert("Il numero di telefono non è corretto");
    return false;
  }

  return true;
};

$(document).ready(function () {
  JQuery();

  $("#button").on("click", function () {
    if (validateForm()) {
      $("#form").submit();
    };
  });
});


const JQuery = () => {
  $("#img1").click(function () {
    $("#p1").toggle();
  });

  $("#img2").click(function () {
    $("#p2").toggle();
  });

  $("#img3").click(function () {
    $("#p3").toggle();
  });

  $("#img4").click(function () {
    $("#p4").toggle();
  });
};