#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class image {
public:
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int** imgAry;
	image(int row, int col, int min, int max, char* inputfilename) {
		numRows = row;
		numCols = col;
		minVal = min;
		maxVal = max;
		makeImgAry(row, col, inputfilename);
	}
	void makeImgAry(int row, int col, char *inputfilename) {
		imgAry = new int*[row];
		for (int i = 0; i < row;i++) {
			imgAry[i] = new int[numCols]();
		}
		loadImage(inputfilename);
	}
	void loadImage(char* input_filename) {
		ifstream image_data(input_filename);
		int skip_line = 0;
		int holder;
		while (skip_line < 4) {
			image_data >> holder;
			skip_line++;
		}
		for (int rowindex = 0; rowindex < numRows; rowindex++) {
			for (int colindex = 0; colindex < numCols; colindex++) {
				image_data >> imgAry[rowindex][colindex];
			}
			cout << endl;
		}
	}
	~image(){}
};
class QuadTreeNode{
public:
	friend class QuadTree;
	friend class image;
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int color; // 0, 1 or 2
	int rowOffset;
	int colOffset;
	QuadTreeNode* NW_kid = NULL; // initialized to NULL
	QuadTreeNode* SW_kid = NULL; // initialized to NULL
	QuadTreeNode* SE_kid = NULL; // initialized to NULL
	QuadTreeNode* NE_kid = NULL; // initialized to NULL
	QuadTreeNode(int row, int col, int rowoff,int coloff,image * our_image) {
		numRows = row;
		numCols = col;
		rowOffset = rowoff;
		colOffset = coloff;
		computeColor(our_image);
		computeMinMaxValue();
}
		void computeColor(image *the_image) {
			int current_num = the_image->imgAry[rowOffset][colOffset];
			if (current_num == 1) { cout << "color is 1 " << rowOffset << " " <<colOffset << " "; }
			for (int ridx = rowOffset; ridx < rowOffset + numRows;ridx++) {
				for (int cidx = colOffset; cidx < colOffset + numCols;cidx++) {
					if (current_num != the_image->imgAry[ridx][cidx]) {
						if (current_num == 1) { cout << ridx << " " << cidx <<" 1 changed" << endl; }
						color = 2;
						return;
					}
				}
			}
			if (current_num == 0) { color = 0; }
			else { color = 1; }
		}

        void computeMinMaxValue() {
					if (color == 2) { maxVal = 1;minVal = 0;}
					else if (color == 1) { maxVal = 1;minVal = 1; }
					else { maxVal = 0;minVal = 1; }
	   }

            void printQuadTreeNode(char* output_filename) {
			ofstream output_file;
			output_file.open(output_filename, fstream::app);
			stringstream output;
			if (color == 2) {
				output << color << "\t" << numRows << "\t" << numCols << "\t" << minVal << "\t" << maxVal << "\t" << rowOffset << "\t" << colOffset << "\t" << NW_kid->color << "\t" << SW_kid->color << "\t" << SE_kid->color << "\t" << NE_kid->color;
				string quad_node = output.str();
				output_file << quad_node << endl;
			}
			else{
				output << color << "\t" << numRows << "\t" << numCols << "\t" << minVal << "\t" << maxVal << "\t" << rowOffset << "\t" << colOffset << "\t" << "NULL" << "\t" << "NULL" << "\t" << "NULL" << "\t" << "NULL";
				string quad_node = output.str();
				output_file << quad_node << endl;
			}
			output_file.close();
		}
			~QuadTreeNode() {};
	  
};



class QuadTree {
public:
	QuadTreeNode * root;
	image * the_image;
	char* output_filename;
	QuadTree(QuadTreeNode * new_root,image * ary_image,char* out_filename) {
		output_filename = out_filename;
		root = new_root;
		the_image = ary_image;
		buildTree(root, the_image);
		printQuadTree(root);
	}
	void buildTree(QuadTreeNode* Node,image *ary_image) {
		if (Node == NULL || Node -> color != 2 ) { return; }
		else {
			Node ->NW_kid = new QuadTreeNode(Node->numRows /2,Node -> numCols / 2,Node->rowOffset , Node->colOffset, ary_image);
			buildTree(Node->NW_kid,ary_image);
            Node ->SW_kid = new QuadTreeNode(Node->numRows /2, Node -> numCols/2,Node -> numRows/2 + Node->rowOffset ,Node ->colOffset, ary_image);
			buildTree(Node->SW_kid, ary_image);
			Node ->SE_kid = new QuadTreeNode(Node->numRows /2, Node -> numCols / 2, Node -> numRows / 2 + Node ->rowOffset,Node -> numCols / 2 + Node ->colOffset, ary_image);
			buildTree(Node->SE_kid, ary_image);
			Node ->NE_kid = new QuadTreeNode(Node->numRows /2, Node -> numCols / 2, Node ->rowOffset, Node-> numCols / 2 + Node ->colOffset , ary_image);
			buildTree(Node->NE_kid, ary_image);
		}


	}

	void printQuadTree(QuadTreeNode *Node) {
		if (Node == NULL) { return; }
		else {
			Node->printQuadTreeNode(output_filename);
			printQuadTree(Node->NW_kid);
			printQuadTree(Node->SW_kid);
			printQuadTree(Node->SE_kid);
			printQuadTree(Node->NE_kid);
		}
	}
	~QuadTree() {};

};

int main(int argc, char* argv[]) {
	char* image_filename = argv[1];
	char* pre_order_filename = argv[2];
	char* debug_filename = argv[3];
	ifstream imagedata(argv[1]);
	ofstream outputfile(pre_order_filename);
	outputfile.close();
	int row; int col; int min; int max;
	imagedata >> row; imagedata >> col; imagedata >> min; imagedata >> max;
	image *newimage = new image(row, col, min, max,image_filename);
	QuadTreeNode * root = new QuadTreeNode(row,col,0,0,newimage);
	QuadTree(root, newimage,pre_order_filename);
    return 0;
}
