from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
from fastapi.middleware.cors import CORSMiddleware
from pathlib import Path
from typing import List


app = FastAPI()


# Enable CORS for all origins
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allow all origins
    allow_credentials=True,
    allow_methods=["*"],  # Allow all HTTP methods
    allow_headers=["*"],  # Allow all headers
)


# Path to where PNG frames are stored
FRAME_DIR = Path("../frames")

@app.get("/frames", response_model=List[str])
def list_frames():
    # Find all PNGs matching "frame_*.png"
    frames = sorted(FRAME_DIR.glob("frame_*.png"), key=lambda f: int(f.stem.split("_")[1]))
    return [frame.name for frame in frames]

@app.get("/frame/{filename}")
def get_frame(filename: str):
    filepath = FRAME_DIR / filename
    if filepath.exists() and filepath.suffix == ".png":
        return FileResponse(path=filepath)
    raise HTTPException(status_code=404, detail="Frame not found")

