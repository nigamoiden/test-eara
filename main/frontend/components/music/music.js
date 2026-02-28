document.addEventListener("DOMContentLoaded", () => {

  const audio = document.getElementById("audioPlayer");
  const sections = document.querySelectorAll(".section");
  const buttons = document.querySelectorAll(".control-btn");

  buttons.forEach(btn => {
    btn.addEventListener("click", () => {
      const target = btn.dataset.section;

      sections.forEach(sec => sec.classList.remove("active"));
      buttons.forEach(b => b.classList.remove("active"));

      document.getElementById(target).classList.add("active");
      btn.classList.add("active");
    });
  });

  async function fetchMusic(query) {
    return new Promise(resolve => {
      setTimeout(() => {
        resolve([
          { title: "Sample Song", file: "../../assets/music.mp3" }
        ]);
      }, 500);
    });
  }

  function saveHistory(song) {
    let history =
      JSON.parse(localStorage.getItem("history")) || [];
    history.push(song);
    localStorage.setItem("history", JSON.stringify(history));
  }

  window.searchMusic = async function () {
    const query = document.getElementById("searchInput").value;
    const results = await fetchMusic(query);

    const container =
      document.getElementById("playlistContainer");

    container.innerHTML = "";

    results.forEach(song => {
      const btn = document.createElement("button");
      btn.innerText = song.title;

      btn.onclick = () => {
        audio.src = song.file;
        audio.play();
        document.getElementById("nowPlaying").innerText =
          song.title;
        saveHistory(song.title);
      };

      container.appendChild(btn);
    });
  };

  window.playMusic = function () {
    audio.play();
    document.getElementById("nowPlaying").innerText =
      "Sample Song";
    saveHistory("Sample Song");
  };

});