function logout() {
  firebase.auth().signOut().then(() => {
    window.location.href = "login.html";
  }).catch((error) => {
    alert("Logout failed: " + error.message);
  });
}
