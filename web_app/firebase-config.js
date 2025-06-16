// firebase-config.js
const firebaseConfig = {
    apiKey: "",
    databaseURL: "",
    projectId: "",
};

// Initialize Firebase if not already initialized
if (!firebase.apps.length) {
    firebase.initializeApp(firebaseConfig);
}
