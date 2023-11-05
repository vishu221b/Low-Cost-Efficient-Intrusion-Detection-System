from pymongo import MongoClient
from .config import MONGO_URI, DB_NAME, COLLECTION_NAME
from .utils import parse_topic_for_collection_name

def get_database():
  
   # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
   client = MongoClient(MONGO_URI)
 
   # Create the database for our example (we will use the same database throughout the tutorial
   return client[DB_NAME]
  
def get_collection(name=COLLECTION_NAME):  
   # Get the database
   if name != COLLECTION_NAME:
      name=parse_topic_for_collection_name(name)
   dbname = get_database()
   return dbname[name]
