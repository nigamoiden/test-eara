document.addEventListener("DOMContentLoaded", () => {

  const track = document.getElementById("track");
  const items = document.querySelectorAll(".nav-item");

  let current = 1;
  let startX = 0;
  let isDragging = false;
  let offset = 0;

  
  function updateCarousel() {
    items.forEach((item, index) => {
      item.classList.remove("active");
      if (index === current) {
        item.classList.add("active");
      }
    });

    const centerOffset =
      window.innerWidth / 2 - items[current].offsetWidth / 2;

    const itemOffset = items[current].offsetLeft;

    track.style.transform = `translateX(${centerOffset - itemOffset}px)`;
  }

  updateCarousel();


  track.addEventListener("mousedown", e => {
    isDragging = true;
    startX = e.clientX;
    track.style.cursor = "grabbing";
  });

  window.addEventListener("mousemove", e => {
    if (!isDragging) return;
    offset = e.clientX - startX;
  });

  window.addEventListener("mouseup", () => {
    if (!isDragging) return;

    isDragging = false;
    track.style.cursor = "grab";

    if (offset < -80 && current < items.length - 1) {
      current++;
    } else if (offset > 80 && current > 0) {
      current--;
    }

    offset = 0;
    updateCarousel();
  });


  items.forEach((item, index) => {
    item.addEventListener("click", () => {
      if (index !== current) {
        current = index;
        updateCarousel();
      } else {
        navigate(item.dataset.route);
      }
    });
  });

  function navigate(route) {
    if (route === "music") {
      window.location.href = "components/music/music.html";
    }
  }

});