// firebase-config.js
const firebaseConfig = {
    apiKey: "api",
    databaseURL: "url",
    projectId: "id",
};

// Initialize Firebase if not already initialized
if (!firebase.apps.length) {
    firebase.initializeApp(firebaseConfig);
}
